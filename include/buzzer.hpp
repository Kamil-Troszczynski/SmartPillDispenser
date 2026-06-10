#pragma once

#include "dfns_consts_libs.hpp"


void buzzer_start(unsigned long durationMs, Adafruit_MCP23X17 & mcp);
void buzzer_update(Adafruit_MCP23X17 & mcp);
void buzz_pattern_mcp(Adafruit_MCP23X17 & mcp);
void handle_buzzer_sound_mcp(Adafruit_MCP23X17 & mcp);