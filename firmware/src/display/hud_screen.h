#pragma once

#include <lvgl.h>
#include "hlp/hlp_state.h"

enum class ScreenState {
    BOOT,
    CONNECTING,
    HUD_ACTIVE,
    NO_SIGNAL
};

class HudScreen {
public:
    static void init();
    static void setScreenState(ScreenState state);
    static void updateData(const HudState& state);

private:
    static void createBootScreen();
    static void createConnectingScreen();
    static void createHudScreen();
    static void createNoSignalScreen();
};
