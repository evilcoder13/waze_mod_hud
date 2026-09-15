#include "display_driver.h"
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "config.h"
#include "hud_screen.h"
#include "utils/nvs_config.h"
#include "utils/logger.h"

static TFT_eSPI s_tft = TFT_eSPI();
static SemaphoreHandle_t s_lvgl_mutex = nullptr;
static lv_disp_draw_buf_t s_draw_buf;
static lv_color_t s_buf1[SCREEN_WIDTH * 10];
static lv_color_t s_buf2[SCREEN_WIDTH * 10];

// Mirror HUD state: 0 = Normal Landscape (Rotation 1), 1 = Mirrored HUD (Rotation 7)
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
    uint16_t touchX = 0, touchY = 0;
    // TFT_eSPI handles calibration and rotation internally
    bool touched = s_tft.getTouch(&touchX, &touchY, 400);

    static bool s_last_touched = false;
    static uint32_t s_touch_press_time = 0;

    if (touched) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;

        if (!s_last_touched) {
            s_touch_press_time = millis();
        }
        s_last_touched = true;
    } else {
        data->state = LV_INDEV_STATE_REL;
        // If user tapped anywhere on screen, allow toggling mirror if desired
        if (s_last_touched) {
            uint32_t press_duration = millis() - s_touch_press_time;
            if (press_duration >= 40 && press_duration <= 1200) {
                // Check if user tapped in top-right or anywhere on the top bar
                if (data->point.y <= 45 || data->point.x >= 220) {
                    LOG_I("Screen tap detected on HUD toggle area! Toggling Mirror HUD mode");
                    DisplayDriver::toggleMirror();
                }
            }
        }
        s_last_touched = false;
    }
}

void DisplayDriver::setBrightness(uint8_t percent) {
    if (percent > 100) percent = 100;
    uint32_t duty = (percent * 255) / 100;
    ledcWrite(BL_PWM_CHANNEL, duty);
}

void DisplayDriver::toggleMirror() {
    s_is_mirrored = !s_is_mirrored;
    if (s_is_mirrored) {
        s_tft.setRotation(7); // Inverted/Mirrored Landscape for windshield reflection
        LOG_I("HUD Mode: Windshield Mirror Reflection (Rotation 7)");
    } else {
        s_tft.setRotation(1); // Normal Landscape
        LOG_I("HUD Mode: Normal Direct View (Rotation 1)");
    }

    if (s_lvgl_mutex && xSemaphoreTake(s_lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
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

    // 1. Configure Touch IRQ pin
    pinMode(PIN_TOUCH_IRQ, INPUT_PULLUP);

    // 2. Backlight PWM setup
    ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ, BL_PWM_RES_BITS);
    ledcAttachPin(PIN_TFT_BL, BL_PWM_CHANNEL);
    setBrightness(NvsConfig::get().brightness);

    // 3. Hardware Display init
    s_tft.init();
    s_tft.setRotation(DISPLAY_ROTATION); // Default: 1 (Landscape)
    s_tft.fillScreen(TFT_BLACK);

    // Touch calibration data for ESP32-2432S028 CYD in landscape
    uint16_t calData[5] = { 260, 3600, 350, 3450, 1 };
    s_tft.setTouch(calData);

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
