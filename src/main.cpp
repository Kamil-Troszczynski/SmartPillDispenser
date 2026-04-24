#include "app_state.hpp"
#include "buttons.hpp"
#include "buzzer.hpp"
#include "screen.hpp"
#include "servos.hpp"
#include "time.hpp"
#include "schedules.hpp"


// Global instances
AppState appState;
TFT_eSPI tft = TFT_eSPI();
ESP32Time rtc(3600);


const int TFT_BL = 27;
Servo servos[NUM_SERVOS];


void setup() {
  Serial.begin(115200);

  ledcAttachPin(TFT_BL, 8);
  ledcWrite(TFT_BL, 200);

  pinMode(BTN_UP,   INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_OK,   INPUT_PULLUP);
  pinMode(BUZZER,   OUTPUT);
  digitalWrite(BUZZER, LOW);

  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(SERVO_START);
  }
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(C_BG);
  sync_time_from_NTP();
  draw_ui();
}


void loop() {
  handle_buttons();
  check_schedules();
  handle_buzzer_sound();

  static unsigned long lastRefresh = 0;
  if (millis() - lastRefresh > 10000) {
    lastRefresh = millis();
    draw_ui();
  }
}