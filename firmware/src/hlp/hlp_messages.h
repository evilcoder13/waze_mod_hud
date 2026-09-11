#pragma once

#include <cstddef>
#include <cstdint>
#include "hlp_state.h"

typedef void (*hlp_send_line_fn)(const char* line, void* user);
typedef void (*hlp_state_update_fn)(const HudState& state, void* user);

void hlp_send_dev(hlp_send_line_fn send, void* user, const char* transport, unsigned rate);
void hlp_handle_line(const char* line, size_t length, hlp_send_line_fn send,
                     hlp_state_update_fn on_state, void* user);
