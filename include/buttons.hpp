#pragma once

#include "dfns_consts_libs.hpp"

void buttons_init_mcp(Adafruit_MCP23X17& mcp);
void buttons_after_wake(Adafruit_MCP23X17& mcp);
void handle_buttons_mcp(Adafruit_MCP23X17& mcp, Adafruit_PWMServoDriver& pca);
