# TEST_PLAN.md — WazeHUD Test Matrix

> Derived from waze-hud-link-sdk-ai-bundle.md Document 12 (TEST_MATRIX.md).
> All cases must be verified on real hardware before firmware is marked production-ready.

---

## Hardware Required

- ESP32-2432S028 (CYD) board x1
- Android phone with Waze Mod installed (Redmi or Pixel preferred)
- USB cable (data-capable)
- Car or powered USB hub
- Windows PC with PlatformIO

---

## Test Environment Setup

```
1. Flash latest firmware build to CYD.
2. Open PlatformIO serial monitor (115200 baud).
3. On Android: adb logcat -s WazeHudLink WazeHudLink-BLE WazeHlp
4. In Waze Mod: enable hud_link_log temporarily during debug.
```

---

## T1 — Basic Connection Tests

| ID | Test | Expected | Pass? |
|----|------|----------|-------|
| T1.1 | Power on CYD → BOOT screen shows | Logo + boot message visible | |
| T1.2 | BT visible in Android settings as "WazeHUD" | Device appears within 10 s | |
| T1.3 | Pair "WazeHUD" from Android settings | Paired successfully, no PIN needed | |
| T1.4 | Select Classic SPP in Waze Mod, enable HUD Link | Serial: "SPP connected", "dev sent", "hi received" | |
| T1.5 | Screen transitions from CONNECTING to HUD_ACTIVE | Screen shows speed/turn within 2 s | |
| T1.6 | Waze Mod shows "Connected" status | Status indicator green | |

---

## T2 — HLP/1 Protocol Tests

| ID | Test | Expected | Pass? |
|----|------|----------|-------|
| T2.1 | Check dev message in adb logcat | {"v":1,"t":"dev","name":"WazeHUD-CYD",...} visible | |
| T2.2 | Check hi response received | Serial: "hi received, rate=8" | |
| T2.3 | Check state stream after handshake | Serial: "HLP state: {v:1,t:s,...}" every ~125ms | |
| T2.4 | Ping sent by Android every 5 s | Serial: "ping received, pong sent" every 5 s | |
| T2.5 | Verify pong sent before render (< 5 ms) | No render delay before pong in logs | |
| T2.6 | ts values monotonically increasing | Each log line shows increasing ts value | |
| T2.7 | Inject malformed JSON (send garbage via BT) | Frame dropped, stream continues | |
| T2.8 | Send 513-byte frame | Oversized counter increments, no crash | |

---

## T3 — Display / HUD Tests

| ID | Test | Expected | Pass? |
|----|------|----------|-------|
| T3.1 | Drive at 47 km/h | Speed shows 47 in white | |
| T3.2 | Speed limit 50 km/h active | Red circle with 50 visible | |
| T3.3 | Exceed speed limit | Speed text turns red | |
| T3.4 | trn=2 (left turn) | Left arrow icon shows | |
| T3.5 | trn=10, exit=3 (roundabout) | Roundabout icon + "3" exit number | |
| T3.6 | trn=17 (arrive) | Arrival icon / flag shows | |
| T3.7 | Unknown trn code (e.g., 99) | No icon, no crash | |
| T3.8 | Street name with Vietnamese diacritics ("Nguyễn Trãi") | Correct Vietnamese display | |
| T3.9 | Long street name (>20 chars) | Truncated with ellipsis | |
| T3.10 | nav=0 (not navigating) | Turn widget hidden, speed full width | |
| T3.11 | ETA shows "20:01", rkm=6.7 | ETA row: "20:01 • 6.7 km" | |
| T3.12 | avg=1, avgL=2000 | No-passing sign + "2.0 km" progress bar | |
| T3.13 | alr=2, alrD=300 | Alert bar shows camera icon + 300 m | |
| T3.14 | alr=0 | Alert bar hidden | |
| T3.15 | alr=99 (unknown) | Generic hazard icon, no crash | |

---

## T4 — Stale / Reconnect Tests

| ID | Test | Expected | Pass? |
|----|------|----------|-------|
| T4.1 | Stop Waze navigation | HUD shows last state, then NO_SIGNAL after 3 s | |
| T4.2 | Close Waze app | NO_SIGNAL screen within 3 s | |
| T4.3 | Re-open Waze, start navigation | HUD_ACTIVE restores within 5 s | |
| T4.4 | Turn off phone Bluetooth | CONNECTING screen shows | |
| T4.5 | Re-enable Bluetooth | Auto-reconnects, HUD_ACTIVE | |
| T4.6 | Walk out of BT range | CONNECTING screen after ~60 s | |
| T4.7 | Return to BT range | Auto-reconnects | |

---

## T5 — WiFi WebSocket Tests

| ID | Test | Expected | Pass? |
|----|------|----------|-------|
| T5.1 | Enable wifi_enabled via config | Serial: "WiFi connected, IP: x.x.x.x" | |
| T5.2 | Select WiFi transport in Waze Mod | HLP stream arrives via WebSocket | |
| T5.3 | Both SPP and WiFi connected | SPP takes priority | |
| T5.4 | SPP disconnects while WiFi active | Switches to WiFi stream | |
| T5.5 | WiFi disconnects | Falls back to SPP if available | |

---

## T6 — Dynamic Config Tests

| ID | Test | Expected | Pass? |
|----|------|----------|-------|
| T6.1 | cfg_begin/item/end sent after hi | Waze Mod shows HUD settings panel | |
| T6.2 | Change brightness slider to 50% | Backlight dims immediately after Apply | |
| T6.3 | Change theme to "night" | LVGL palette switches to night colors | |
| T6.4 | Change theme to "day" | LVGL palette switches to day colors | |
| T6.5 | Reconnect after config change | New config persists (loaded from NVS) | |
| T6.6 | Send invalid brightness (101) | cfg_ack ok=false, config unchanged | |
| T6.7 | Power cycle after config change | Settings survive reboot | |

---

## T7 — Stress / Long-Running Tests

| ID | Test | Duration | Expected | Pass? |
|----|------|----------|----------|-------|
| T7.1 | Continuous navigation stream | 15 min | No crash, no memory leak, ping/pong every 5 s | |
| T7.2 | Repeated connect/disconnect | 20 cycles | Always reconnects cleanly | |
| T7.3 | BT toggle off/on | 10 cycles | Reconnects each time, no stack corruption | |
| T7.4 | Overnight connected (no navigation) | 8 hours | No crash, LVGL stable | |

---

## T8 — Android Version Tests

| Android version | BT pairing | SPP stream | BLE (if added) | Pass? |
|-----------------|-----------|-----------|----------------|-------|
| Android 10 | | | | |
| Android 11 | | | | |
| Android 12 | | | | |
| Android 13 | | | | |
| Android 14 | | | | |

---

## Test Log Template

For each hardware test session, record:

```
Date: 2026-xx-xx
Firmware version: x.x.x
Board revision: ESP32-2432S028 vX
Android version: XX
Transport: Classic SPP / WiFi WS
Negotiated rate: X Hz
Test result: PASS / FAIL
Failed tests: T?.?
Failure details: ...
Notes: ...
```
