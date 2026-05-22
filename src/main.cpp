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

Adafruit_PWMServoDriver pca;


void setup() {
  Serial.begin(9600);

  // Initialize PCA9685 and set up servos
  setup_up_servos(pca);
}


void loop() {
  // Second servo test
  move_servo(2, pca);
}