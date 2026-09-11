#pragma once

#include <cstddef>
#include <cstdint>
#include <ArduinoJson.h>

typedef void (*hlp_send_line_t)(const char* line, void* user);

void hlp_device_config_init();
void hlp_device_config_publish(hlp_send_line_t send, void* user);
bool hlp_device_config_handle(const JsonObjectConst& root, hlp_send_line_t send, void* user);
