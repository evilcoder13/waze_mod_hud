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
static lv_color_t s_buf1[SCREEN_WIDTH * 20];
static lv_color_t s_buf2[SCREEN_WIDTH * 20];

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    s_tft.startWrite();
    s_tft.setAddrWindow(area->x1, area->y1, w, h);
    s_tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    s_tft.endWrite();

    lv_disp_flush_ready(disp);
}

void DisplayDriver::setBrightness(uint8_t percent) {
    if (percent > 100) percent = 100;
    uint32_t duty = (percent * 255) / 100;
    ledcWrite(BL_PWM_CHANNEL, duty);
}

static void renderTask(void* param) {
    LOG_I("renderTask started on Core %d", xPortGetCoreID());
    while (true) {
        if (s_lvgl_mutex && xSemaphoreTake(s_lvgl_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            lv_timer_handler();
            xSemaphoreGive(s_lvgl_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool DisplayDriver::init(SemaphoreHandle_t lvgl_mutex) {
    s_lvgl_mutex = lvgl_mutex;

    // 1. Backlight PWM setup
    ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ, BL_PWM_RES_BITS);
    ledcAttachPin(PIN_TFT_BL, BL_PWM_CHANNEL);
    setBrightness(NvsConfig::get().brightness);

    // 2. Hardware Display init
    s_tft.init();
    s_tft.setRotation(DISPLAY_ROTATION); // Landscape
    s_tft.fillScreen(TFT_BLACK);

    // 3. LVGL Init
    lv_init();
    lv_disp_draw_buf_init(&s_draw_buf, s_buf1, s_buf2, SCREEN_WIDTH * 20);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &s_draw_buf;
    lv_disp_drv_register(&disp_drv);

    // 4. Initialize Screen elements
    HudScreen::init();

    LOG_I("Display and LVGL initialized successfully (240x320)");
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
