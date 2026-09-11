# PROTOCOL_NOTES.md — HLP/1 Implementation Notes

> Implementation guidance derived from waze-hud-link-sdk-ai-bundle.md.
> These notes address edge cases not obvious from the SDK alone.

---

## 1. Frame Framing Rules

### Buffer management
- Allocate ONE static buffer of 512 bytes in `hlp_core.cpp`: `static uint8_t s_buf[512]`.
- Never allocate per-frame buffers dynamically.
- `length` counter resets on every `\n`.
- On overflow (length >= 511 before `\n`): set `overflow=true`, increment `oversized` counter, continue consuming bytes until `\n`, then reset.
- On `\r\n` line endings: strip the `\r` before calling the line callback.

### UTF-8 validation
- Run `hlp_utf8_valid()` on every assembled line before invoking the callback.
- Invalid UTF-8 → increment `malformed_utf8`, skip line, reset buffer.
- Do NOT validate while assembling — only on complete lines.

### BLE-specific (if BLE added later)
- BLE ATT notifications may split one JSON frame across multiple write events.
- Treat each write as a raw byte stream — do NOT assume write boundary = frame boundary.
- The `\n` delimiter is the only valid frame boundary.

---

## 2. Handshake Edge Cases

### `dev` sent too early
- If SPP callback fires before the queue/task is initialized, dev send may crash.
- Solution: send `dev` from `protocol_task` when it sees `LINK_SPP_UP` event, NOT from the SPP callback.

### `dev` sent too late (after 500 ms)
- Waze Mod will enter legacy fallback mode and send `hi` + `s` without waiting.
- When our `dev` finally arrives, Waze re-negotiates and sends a new `hi`.
- `hi` is idempotent — accept it and update session state. No reconnect needed.

### Multiple `hi` messages
- Android can send `hi` again after link events without disconnecting.
- On each new `hi`:
  1. Parse `sess` field.
  2. If `sess` differs from previous: clear `g_hud_state` (memset to 0).
  3. Update negotiated `rate` and `fields` list.
  4. Do NOT restart Bluetooth stack.

### `sess` field missing from `hi`
- Old Waze Mod versions may not include `sess`.
- If missing: treat as sess=0, do NOT clear state (backward compatible).

---

## 3. State Message (`t="s"`) Parsing

### Missing fields = default values
```cpp
// ArduinoJson: use | operator for defaults
state.spd  = doc["spd"]  | 0;
state.lim  = doc["lim"]  | 0;
state.dst  = doc["dst"]  | -1;     // -1 = no turn imminent
state.alrD = doc["alrD"] | -1;     // -1 = no alert
state.trn  = doc["trn"]  | 0;
state.nav  = doc["nav"]  | 0;
```

### rkm as number
- `rkm` may serialize as integer when value is whole: `6` instead of `6.0`.
- Always read as `float` or `double`, never `int`:
  ```cpp
  state.rkm = doc["rkm"] | 0.0f;
  ```

### String fields
- `st` and `st2` are UTF-8, may contain Vietnamese with diacritics.
- Always `strlcpy` into `char[41]` buffers (40 code points + NUL).
  ```cpp
  const char* st = doc["st"] | "";
  strlcpy(state.st, st, sizeof(state.st));
  ```

### `alrs` array (opt-in)
- Only sent if `alrs` was in `dev.want.fields` (it is, per GEMINI.md §4.3).
- After negotiation: producer sends `"alrs":[]` when no alerts active (empty array, not missing key).
- Missing `alrs` key = field not negotiated or frame fallback — use `alr`/`alrD`/`alrV` instead.
- Parse safely:
  ```cpp
  state.alrs_count = 0;
  JsonArray alrs = doc["alrs"];
  for (JsonObject entry : alrs) {
      if (state.alrs_count >= 4) break;
      state.alrs[state.alrs_count++] = {
          entry["k"] | 0,
          entry["d"] | -1,
          entry["v"] | 0
      };
  }
  ```

### `lan` array (lane guidance)
- Opt-in, complex bitmask structure.
- Parse only if display supports lane guidance (future feature).
- v1.0: ignore `lan` field to save parsing time.

---

## 4. Ping/Pong Timing

- Android sends `ping` every **5 seconds**.
- ESP32 MUST reply with `pong` **immediately** — in `protocol_task`, before any other processing.
- Do NOT queue `pong` for later — send synchronously in the message handler.
- Pong format: `{"v":1,"t":"pong"}\n` (with LF).

### What "immediately" means in code:
```cpp
// In hlp_handle_line(), called from protocol_task:
if (strcmp(type, "ping") == 0) {
    send_line("{\"v\":1,\"t\":\"pong\"}");
    return;  // return before any slow work
}
```

---

## 5. Stale Detection

- Update `g_hud_state.last_rx_ms = millis()` on EVERY received message (not just `s`).
- In `protocol_task`, check every 100 ms:
  ```cpp
  if (millis() - g_hud_state.last_rx_ms > 3000) {
      xEventGroupSetBits(g_link_event_group, LINK_STALE);
  }
  ```
- When `LINK_STALE` is set, `render_task` transitions to `NO_SIGNAL` screen.
- Clear `LINK_STALE` on next received message.

---

## 6. Vietnam-Specific: avg=1 Rendering

> WARNING: This is a common mistake. Read carefully.

In Vietnam, Waze editors use the "average speed camera zone" geometry to mark **no-passing zones**.
The HLP/1 SDK documents this explicitly.

**What to render when `avg=1`:**
- Show: **no-passing sign** (Vietnam standard: round red border, two cars side by side)
- Show: countdown `avgL` in metres (how far until zone ends)
- Show: `avgR` as recommended speed
- **DO NOT show**: speed camera icon
- **DO NOT show**: average speed calculation

**Fields:**
```
avg=1    : we are INSIDE a no-passing zone
avgL     : metres remaining in zone (counts down)
avgR     : recommended speed km/h
avgP     : progress through zone (0-100%)
```

---

## 7. Alert Code Fallback

```cpp
// In alert_screen.cpp or hud_screen.cpp
const lv_img_dsc_t* get_alert_icon(int alr_code) {
    switch (alr_code) {
        case 1:  return &img_police;
        case 2:  return &img_speed_camera;
        case 3:  return &img_redlight_camera;
        case 4:  return &img_hazard;
        case 5:  return &img_accident;
        case 6:  return &img_traffic;
        case 7:  return &img_road_closed;
        case 8:  return &img_speed_drop;
        case 9:  return &img_no_passing;
        // Codes 10-74: extended, map to closest category or generic
        default: return &img_hazard;  // REQUIRED fallback for unknown codes
    }
}
```

---

## 8. `ts` (Timestamp) Ordering

- `ts` is producer **uptime in milliseconds**, NOT wall clock time.
- Use `(sess, ts)` pair together for ordering:
  - If `sess` changed: always accept (new session).
  - If `sess` same and `ts` > last: accept (newer frame).
  - If `sess` same and `ts` <= last: discard (duplicate or reorder).
- If producer process restarts, `sess` changes — do not use `ts` alone.

---

## 9. Dynamic Config (cfg_*) Sequence

### Publish schema after `hi`
```
1. Receive hi from Android
2. Call hlp_device_config_publish(send_line, nullptr)
3. This sends: cfg_begin → N x cfg_item → cfg_end
4. All must have same rev number
5. Android shows UI only after receiving complete schema
```

### Commit atomically
```
1. Receive cfg_set_begin → check tx, rev, count → copy active to draft
2. Receive N x cfg_set → validate each → write to draft only
3. Receive cfg_set_commit → verify all fields received once
4. Save draft to NVS (Preferences) atomically
5. Only THEN: active = draft, rev++, send cfg_ack ok=true
6. If any step fails: send cfg_ack ok=false, reset transaction
```

### DO NOT
- Do not apply config changes to display/hardware until after successful cfg_ack.
- Do not send cfg_ack before NVS write completes.
- Do not modify active config if NVS write fails.
