#include "display_driver.h"
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "config.h"
#include "hud_screen.h"
#include "utils/nvs_config.h"
#include "utils/logger.h"

static TFT_eSPI s_tft = TFT_eSPI();
static SPIClass s_touch_spi = SPIClass(VSPI);
// Use Polling Mode (pass only CS pin) to avoid GPIO 36 IRQ lockups when WiFi/BT are active
static XPT2046_Touchscreen s_ts(PIN_TOUCH_CS);

// Calibration range for 2.8" CYD resistive touchscreen (ESP32-2432S028 G205)
static uint16_t s_touch_min_x = 200, s_touch_max_x = 3700;
static uint16_t s_touch_min_y = 240, s_touch_max_y = 3800;

static SemaphoreHandle_t s_lvgl_mutex = nullptr;
static lv_disp_draw_buf_t s_draw_buf;
static lv_color_t s_buf1[SCREEN_WIDTH * 10];
static lv_color_t s_buf2[SCREEN_WIDTH * 10];

// Mirror HUD state: 0 = Normal Landscape (Rotation 1), 1 = Mirrored HUD (Rotation 5 - Flip Ngang)
static bool s_is_mirrored = false;

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    s_tft.startWrite();
    s_tft.setAddrWindow(area->x1, area->y1, w, h);
    s_tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    s_tft.endWrite();

    lv_disp_flush_ready(disp);
}

// Touch input read callback for XPT2046 sending points to LVGL
static void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    static bool s_last_touched = false;
    static uint32_t s_touch_press_time = 0;
    static int16_t s_last_x = 0, s_last_y = 0;
    static uint32_t s_last_debug_ms = 0;

    // Direct check on XPT2046
    bool is_touched = s_ts.touched();

    if (is_touched) {
        TS_Point p = s_ts.getPoint();

        // Filter false touches or noise
        if (p.z > 150) {
            // Dynamic auto-calibration bounds
            if (p.x < s_touch_min_x && p.x > 30)   s_touch_min_x = p.x;
            if (p.x > s_touch_max_x && p.x < 4080) s_touch_max_x = p.x;
            if (p.y < s_touch_min_y && p.y > 30)   s_touch_min_y = p.y;
            if (p.y > s_touch_max_y && p.y < 4080) s_touch_max_y = p.y;

            int16_t px = map(p.x, s_touch_min_x, s_touch_max_x, 0, SCREEN_WIDTH);
            int16_t py = map(p.y, s_touch_min_y, s_touch_max_y, 0, SCREEN_HEIGHT);

            if (px < 0) px = 0;
            if (px >= SCREEN_WIDTH) px = SCREEN_WIDTH - 1;
            if (py < 0) py = 0;
            if (py >= SCREEN_HEIGHT) py = SCREEN_HEIGHT - 1;

            // When in mirrored HUD mode, invert X so touch aligns with optically flipped display
            if (s_is_mirrored) {
                px = SCREEN_WIDTH - 1 - px;
            }

            data->point.x = px;
            data->point.y = py;
            data->state = LV_INDEV_STATE_PR;

            s_last_x = px;
            s_last_y = py;

            uint32_t now = millis();
            if (now - s_last_debug_ms > 250) {
                s_last_debug_ms = now;
                LOG_I("TOUCH PRESSED: raw(x=%d, y=%d, z=%d) -> px=%d, py=%d", p.x, p.y, p.z, px, py);
            }

            if (!s_last_touched) {
                s_touch_press_time = millis();
            }
            s_last_touched = true;
            return;
        }
    }

    data->state = LV_INDEV_STATE_REL;
    // If user tapped on top bar or HUD toggle area, toggle mirror mode
    if (s_last_touched) {
        uint32_t press_duration = millis() - s_touch_press_time;
        LOG_I("TOUCH RELEASED after %u ms at last pos (%d, %d)", press_duration, s_last_x, s_last_y);
        if (press_duration >= 40 && press_duration <= 1500) {
            if (s_last_y <= 60 || s_last_x >= 210) {
                LOG_I("Touch tap on HUD toggle button recognized! Toggling Mirror HUD mode");
                DisplayDriver::toggleMirror();
            }
        }
    }
    s_last_touched = false;
}

void DisplayDriver::setBrightness(uint8_t percent) {
    if (percent > 100) percent = 100;
    uint32_t duty = (percent * 255) / 100;
    ledcWrite(BL_PWM_CHANNEL, duty);
}

void DisplayDriver::toggleMirror() {
    s_is_mirrored = !s_is_mirrored;
    s_tft.startWrite();
    s_tft.writecommand(TFT_MADCTL);
    if (s_is_mirrored) {
        // Horizontal Mirror Flip (FLIP NGANG) for windshield reflection:
        // MX=1, MY=1, MV=1 (0xE8) -> Inverts left/right so mirror reflections read correctly
        s_tft.writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_COLOR_ORDER);
        LOG_I("HUD Mode: Windshield Mirror Reflection (Horizontal Flip MADCTL 0xE8)");
    } else {
        // Normal Landscape Direct View: MX=1, MY=0, MV=1 (0x68)
        s_tft.writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_COLOR_ORDER);
        LOG_I("HUD Mode: Normal Direct View (Landscape MADCTL 0x68)");
    }
    s_tft.endWrite();

    if (s_lvgl_mutex && xSemaphoreTake(s_lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        HudScreen::updateMirrorButton(s_is_mirrored);
        lv_obj_invalidate(lv_scr_act());
        xSemaphoreGive(s_lvgl_mutex);
    }
}

bool DisplayDriver::isMirrored() {
    return s_is_mirrored;
}

static void renderTask(void* param) {
    LOG_I("renderTask started on Core %d", xPortGetCoreID());

    // Hardware BOOT button setup (GPIO 0, active LOW with internal pull-up)
    pinMode(PIN_BUTTON_BOOT, INPUT_PULLUP);
    bool last_btn_state = HIGH;
    uint32_t last_btn_press_time = 0;

    while (true) {
        // Poll physical BOOT button for instant HUD mirror toggle
        bool btn_state = digitalRead(PIN_BUTTON_BOOT);
        if (btn_state == LOW && last_btn_state == HIGH) {
            uint32_t now = millis();
            if (now - last_btn_press_time > 300) { // Debounce 300ms
                LOG_I("Hardware BOOT button pressed! Toggling HUD mirror mode");
                DisplayDriver::toggleMirror();
                last_btn_press_time = now;
            }
        }
        last_btn_state = btn_state;

        if (s_lvgl_mutex && xSemaphoreTake(s_lvgl_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            lv_timer_handler();
            xSemaphoreGive(s_lvgl_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool DisplayDriver::init(SemaphoreHandle_t lvgl_mutex) {
    s_lvgl_mutex = lvgl_mutex;

    // 1. Configure and Initialize Touch Hardware via XPT2046 on dedicated VSPI bus
    pinMode(PIN_TOUCH_IRQ, INPUT_PULLUP);
    s_touch_spi.begin(PIN_TOUCH_CLK, PIN_TOUCH_MISO, PIN_TOUCH_MOSI, PIN_TOUCH_CS);
    s_ts.begin(s_touch_spi);
    s_ts.setRotation(1); // Matches Landscape orientation
    LOG_I("XPT2046 Touch controller initialized on VSPI (CLK=%d, MISO=%d, MOSI=%d, CS=%d, IRQ=%d)",
          PIN_TOUCH_CLK, PIN_TOUCH_MISO, PIN_TOUCH_MOSI, PIN_TOUCH_CS, PIN_TOUCH_IRQ);

    // 2. Backlight PWM setup
    ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ, BL_PWM_RES_BITS);
    ledcAttachPin(PIN_TFT_BL, BL_PWM_CHANNEL);
    setBrightness(NvsConfig::get().brightness);

    // 3. Hardware Display init
    s_tft.init();
    s_tft.setRotation(DISPLAY_ROTATION); // Default: 1 (Landscape)
    s_tft.fillScreen(TFT_BLACK);

    // 4. LVGL Init
    lv_init();
    lv_disp_draw_buf_init(&s_draw_buf, s_buf1, s_buf2, SCREEN_WIDTH * 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &s_draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Register touch input driver to LVGL indev pointer
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // 5. Initialize Screen elements
    HudScreen::init();

    LOG_I("Display and Touch initialized successfully (320x240)");
    return true;
}

void DisplayDriver::startRenderTask() {
    xTaskCreatePinnedToCore(
        renderTask,
        "render",
        STACK_RENDER,
        nullptr,
        PRIO_RENDER,
        nullptr,
        CORE_RENDER
    );
}
