// Include all libraries
#include <ESP32Servo.h>
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <ESP32Time.h>

#ifndef DFNS_CONSTS_DEFINED
#define DFNS_CONSTS_DEFINED

// General defines
#define SCREEN_W 160
#define SCREEN_H 128
#define C_BG         TFT_WHITE
#define C_HEADER_BG  0x1C9F
#define C_HEADER_TXT TFT_WHITE
#define C_ROW_ODD    0xEF7D
#define C_ROW_EVEN   TFT_WHITE
#define C_SELECT_BG  0xC6FF
#define C_SELECT_ACC 0x025F
#define C_TEXT       0x1082
#define C_MUTED      0x8410
#define C_ACTIVE_BG  0x2DC6
#define C_ACTIVE_TXT TFT_WHITE
#define C_WARN_BG    0xFD20
#define C_WARN_TXT   TFT_WHITE
#define C_FOOTER_BG  0xC638
#define C_FOOTER_TXT 0x4208
#define C_SEP        0xAD75
#define C_SCROLLBG   0xD69A
#define C_SCROLLFG   0x025F


// Screen consts
const int HEADER_H = 18;
const int FOOTER_Y = 118;
const int FOOTER_H = 10;
const int ROW_H = 33;
const int VISIBLE_ROWS = 3;
const int LIST_Y = HEADER_H;


// Buttons
const int BTN_UP   = 32;
const int BTN_DOWN = 33;
const int BTN_OK   = 25;


// Buzzer
const int BUZZER   = 26;
const unsigned long DEBOUNCE_MS = 250;


// Person number
const int NUM_PERSONS = 3;


// Servos
const int NUM_SERVOS             = 4;
const int SERVO_PINS[NUM_SERVOS] = { 13, 12, 14, 19 };
const int SERVO_START            = 0;
const int SERVO_END              = 180;

#endif