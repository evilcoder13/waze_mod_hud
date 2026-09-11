# DISPLAY_LAYOUT.md — WazeHUD Screen Layout

> Coordinate reference for `hud_screen.h`. All pixel offsets must be defined as `constexpr` constants.
> Screen: 240 × 320 px, portrait, ILI9341. LVGL v8.3.x, RGB565 color.

---

## Main HUD Screen Wireframe

```
x=0                    x=240
y=0  ┌────────────────────────────┐
     │  ⚠ CAMERA    <- 300m       │ ← Alert Bar (32px, hidden if alr=0)
     ├────────────────────────────┤
y=32 │ SPEED   │  TURN ARROW  │
     │         │  96x96 px    │
     │  [ 47 ] │      ↰       │ ← Turn icon (96x96)
     │  km/h   │              │
     │  ────── │  <- 250 m    │ ← dst (distance to turn)
     │  [50]   │  Khuất Duy  │ ← st2 (next street)
     │         │  Tiến       │
     ├─────────┴───────────────┤
y=224│  Nguyễn Trãi             │ ← st (current street, 28px)
     ├────────────────────────────┤
y=252│  ETA 20:01    còn 6.7km   │ ← eta + rkm (28px)
     ├────────────────────────────┤
y=280│  █████████░░ CẤM VƯỢT 2km  │ ← avgzone bar (40px, hidden if avg=0)
y=320└────────────────────────────┘
```

---

## Zone Definitions (constexpr in hud_screen.h)

```cpp
// Alert bar
constexpr int ZONE_ALERT_X = 0,   ZONE_ALERT_Y = 0;
constexpr int ZONE_ALERT_W = 240, ZONE_ALERT_H = 32;

// Main area
constexpr int ZONE_MAIN_Y  = 32,  ZONE_MAIN_H  = 192; // y=32..223

// Speed widget (left half)
constexpr int ZONE_SPEED_X = 0,   ZONE_SPEED_Y = 32;
constexpr int ZONE_SPEED_W = 120, ZONE_SPEED_H = 192;

// Turn widget (right half)
constexpr int ZONE_TURN_X  = 120, ZONE_TURN_Y  = 32;
constexpr int ZONE_TURN_W  = 120, ZONE_TURN_H  = 192;

// Turn arrow icon
constexpr int ICON_TURN_X  = 132, ICON_TURN_Y  = 40;
constexpr int ICON_TURN_W  = 96,  ICON_TURN_H  = 96;

// Current street
constexpr int ZONE_STREET_X = 0,  ZONE_STREET_Y = 224;
constexpr int ZONE_STREET_W = 240,ZONE_STREET_H = 28;

// ETA row
constexpr int ZONE_ETA_X   = 0,   ZONE_ETA_Y   = 252;
constexpr int ZONE_ETA_W   = 240, ZONE_ETA_H   = 28;

// Avg-zone bar
constexpr int ZONE_AVG_X   = 0,   ZONE_AVG_Y   = 280;
constexpr int ZONE_AVG_W   = 240, ZONE_AVG_H   = 40;
```

---

## Widget Details

### Speed Widget (left 120px, y=32..223)

| Element | Position | Font | Color |
|---------|----------|------|-------|
| Speed number | center, large | 48px | white (normal), red (over limit) |
| "km/h" label | below number | 14px | gray |
| Speed limit circle | bottom-left, 40x40px | 16px | white on red bg |
| Separator line | vertical at x=118 | 1px | gray |

- Speed > limit + margin → change speed text to `COLOR_SPEED_OVER` (#FF4444).
- Speed limit = 0 → hide limit circle.

### Turn Widget (right 120px, y=32..223)

| Element | Position | Size | Notes |
|---------|----------|------|-------|
| Turn arrow icon | top-right, x=132 y=40 | 96x96 px | Bitmap from turn_icons.h |
| Distance text | below icon | 16px | "<- 250 m" or "250 m ->" |
| Next street | below distance | 16px | st2, max 2 lines, clip |
| Roundabout exit | overlay on icon | 24px bold | when trn=10/11/12/20, show exit number |

- trn=0 (NONE): hide entire turn widget, expand speed widget to full width.
- trn=17 (ARRIVE): show checkered flag icon.

### Alert Bar (y=0..31, full width)

| Element | Position | Notes |
|---------|----------|-------|
| Alert icon | left, 24x24px | icon by alr code |
| Alert type text | center | e.g., "Camera", "Cảnh sát" |
| Distance | right | "<- 300m" |
| Speed value | after type | only for SPEED_DROP (alrV) |

- Hidden (height=0) when alr=0 or alrD=-1.
- Background color by severity:
  - Police (1), Camera (2,3,46): `#F44336` red
  - Hazard/Accident (4,5): `#FF9800` orange  
  - Traffic (6): `#FFC107` yellow
  - Other: `#FF9800` orange

### Current Street (y=224..251)
- Full width, centered text.
- Font 24px, color `#E0E0E0`.
- Truncate with ellipsis if too long.

### ETA Row (y=252..279)
- Left: `ETA 20:01` (font 16px)
- Right: `còn 6.7 km` (font 16px)
- Center: `|` separator
- Hidden when nav=0 (not navigating).

### Avg-Zone Bar (y=280..319)
- Visible only when avg=1.
- Progress bar fills left-to-right, showing avgP percent.
- Text overlay: `CẤM VƯỢT ← 2 km` (avgL metres converted to km).
- Color: `#FF5722` deep orange.
- When avg=0: bar height = 0, ETA row expands to y=252..319.

---

## Color Palette

```cpp
// Night mode (default)
constexpr uint32_t COLOR_BG          = 0x1A1A2E;  // navy dark
constexpr uint32_t COLOR_SPEED_NORMAL = 0xFFFFFF;  // white
constexpr uint32_t COLOR_SPEED_OVER   = 0xFF4444;  // red
constexpr uint32_t COLOR_TURN_ARROW   = 0x4FC3F7;  // light blue
constexpr uint32_t COLOR_STREET       = 0xE0E0E0;  // light gray
constexpr uint32_t COLOR_LABEL        = 0x9E9E9E;  // dim gray
constexpr uint32_t COLOR_LIMIT_BG     = 0xF44336;  // red circle
constexpr uint32_t COLOR_LIMIT_TEXT   = 0xFFFFFF;  // white
constexpr uint32_t COLOR_ALERT_POLICE = 0xF44336;  // red
constexpr uint32_t COLOR_ALERT_CAMERA = 0xF44336;  // red
constexpr uint32_t COLOR_ALERT_OTHER  = 0xFF9800;  // orange
constexpr uint32_t COLOR_AVG_BAR      = 0xFF5722;  // deep orange
constexpr uint32_t COLOR_SEPARATOR    = 0x424242;  // dark gray

// Day mode (applied when theme="day" or auto+bright ambient)
constexpr uint32_t COLOR_BG_DAY      = 0xF5F5F5;  // light gray
constexpr uint32_t COLOR_SPEED_DAY   = 0x212121;  // near-black
constexpr uint32_t COLOR_STREET_DAY  = 0x424242;  // dark gray
```

---

## Connect Screen

```
┌────────────────────────────┐
│                            │
│     [Bluetooth icon]        │  ← animated pulse
│                            │
│  Đang tìm Waze Mod...      │  ← 24px
│                            │
│  Kết nối: WazeHUD         │  ← 16px, device name
│  Transport: Bluetooth SPP  │  ← 14px, dimmed
│                            │
│  IP: 192.168.1.xxx         │  ← 14px, if WiFi active
│                            │
└────────────────────────────┘
```

## No Signal Screen

```
┌────────────────────────────┐
│                            │
│      MẤT TÍN HIỆU          │  ← red, 24px
│                            │
│  Kết nối BT vẫn hoạt     │  ← 16px
│  nhưng Waze chưa gửi      │
│  dữ liệu. Mở Waze và     │
│  bật Điều hướng.          │
│                            │
└────────────────────────────┘
```

---

## Turn Icon Sizes

All 20 turn icons are 96x96 px monochrome bitmaps stored as `const uint8_t[]` arrays in `turn_icons.h`.

| trn code | Icon name | Description |
|---:|---|---|
| 0 | NONE | (no icon) |
| 1 | STRAIGHT | Straight arrow up |
| 2 | LEFT | Left arrow |
| 3 | RIGHT | Right arrow |
| 4 | SLIGHT_LEFT | Angled left |
| 5 | SLIGHT_RIGHT | Angled right |
| 6 | SHARP_LEFT | Sharp left |
| 7 | SHARP_RIGHT | Sharp right |
| 8 | U_TURN | U-turn arrow |
| 10 | ROUNDABOUT | Circle arrow + exit num |
| 11 | ROUNDABOUT_L | Circle left |
| 12 | ROUNDABOUT_R | Circle right |
| 13 | KEEP_LEFT | Fork left |
| 14 | KEEP_RIGHT | Fork right |
| 15 | EXIT_LEFT | Exit ramp left |
| 16 | EXIT_RIGHT | Exit ramp right |
| 17 | ARRIVE | Checkered flag / pin |
| 19 | RB_STRAIGHT | Roundabout straight |
| 20 | RB_UTURN | Roundabout U-turn |
| default | HAZARD | Generic warning (fallback) |
