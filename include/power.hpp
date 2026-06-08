#pragma once

#include <Adafruit_MCP23X17.h>

void power_init(Adafruit_MCP23X17& mcp);
void power_tick(Adafruit_MCP23X17& mcp);
void power_notify_user_activity();
void power_ensure_display_on();
bool power_should_poll_buttons();
