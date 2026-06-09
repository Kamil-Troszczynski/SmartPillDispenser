#pragma once

#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_MCP23X17.h>
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <Wire.h>
#include <ESP32Time.h>

#ifndef DFNS_CONSTS_DEFINED
#define DFNS_CONSTS_DEFINED

#define SCREEN_W 160
#define SCREEN_H 128

#define C_BG         TFT_WHITE
#define C_SLEEP      TFT_BLACK
#define C_HEADER_BG  0x1C9F
#define C_HEADER_TXT TFT_WHITE
#define C_ROW_ODD    0xEF7D
#define C_ROW_EVEN   TFT_WHITE
#define C_SELECT_BG  0xC6FF
#define C_SELECT_ACC 0x025F
#define C_TEXT       0x1082
#define C_ACTIVE_BG  0x2DC6
#define C_ACTIVE_TXT TFT_WHITE
#define C_WARN_BG    0xFD20
#define C_WARN_TXT   TFT_WHITE
#define C_FOOTER_BG  0xC638
#define C_FOOTER_TXT 0x4208
#define C_SEP        0xAD75
#define C_SCROLLBG   0xD69A
#define C_SCROLLFG   0x025F

#define MCP_BTN_UP   0
#define MCP_BTN_DOWN 1
#define MCP_BTN_OK   2
#define MCP_BUZZER   8
#define MCP_INT_PIN  15

const int HEADER_H = 18;
const int FOOTER_Y = 118;
const int FOOTER_H = 10;
const int ROW_H = 33;
const int VISIBLE_ROWS = 3;
const int LIST_Y = HEADER_H;

const int DETAIL_LIST_Y       = HEADER_H + 4 + 14 + 4;
const int DETAIL_ACTION_Y     = FOOTER_Y - 22;
const int DETAIL_LIST_BOTTOM  = DETAIL_ACTION_Y - 4;
const int DETAIL_ROW_PITCH    = 20;
const int DETAIL_ROW_H        = 18;
const int VISIBLE_DETAIL_ROWS = max(1, (DETAIL_LIST_BOTTOM - DETAIL_LIST_Y) / DETAIL_ROW_PITCH);

const unsigned long DEBOUNCE_MS = 250;

const int NUM_DISPENSERS = 4;
const int SERVO_CHANNELS[NUM_DISPENSERS] = { 0, 4, 8, 12 };
const int SENSOR_PINS[NUM_DISPENSERS]    = { 25, 33, 32, 35 };

const int SERVO_FREQ      = 50;
const int SERVO_MIN_PULSE = 100;
const int SERVO_MAX_PULSE = 500;

#endif
