#pragma once

#include <Adafruit_MCP23X17.h>
#include <Adafruit_PWMServoDriver.h>

void power_init(Adafruit_MCP23X17& mcp);
void power_tick(Adafruit_MCP23X17& mcp, Adafruit_PWMServoDriver& pca);
void power_notify_user_activity();
void power_ensure_display_on();
bool power_should_poll_buttons();
