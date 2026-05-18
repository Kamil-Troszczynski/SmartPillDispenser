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


// void setup() {
//   Serial.begin(115200);

//   ledcAttachPin(TFT_BL, 8);
//   ledcWrite(TFT_BL, 200);

//   pinMode(BTN_UP,   INPUT_PULLUP);
//   pinMode(BTN_DOWN, INPUT_PULLUP);
//   pinMode(BTN_OK,   INPUT_PULLUP);
//   pinMode(BUZZER,   OUTPUT);
//   digitalWrite(BUZZER, LOW);

//   for (int i = 0; i < NUM_SERVOS; i++) {
//     servos[i].attach(SERVO_PINS[i]);
//     servos[i].write(SERVO_START);
//   }
//   tft.init();
//   tft.setRotation(1);
//   tft.fillScreen(C_BG);
//   sync_time_from_NTP();
//   draw_ui();
  
// }


// void loop() {
//   // handle_buttons();
//   // check_schedules();
//   // handle_buzzer_sound();

//   // static unsigned long lastRefresh = 0;
//   // if (millis() - lastRefresh > 10000) {
//   //   lastRefresh = millis();
//   //   draw_ui();
//   // }
// }


void setup() {
  Serial.begin(9600);
  Serial.println("Alternate Servo Test");

  pca.begin();
  pca.setPWMFreq(50);

  for (int i = 0; i < NUM_SERVOS; i++) {
    pca.setPWM(SERVO_CHANNELS[i], 0, SERVO_MIN_PULSE);
  }
}


void loop() {
  // Rotate all servos to one extreme position
  for (int pos = SERVO_MIN_PULSE; pos <= SERVO_MAX_PULSE; pos += 1) {
    for (int i = 0; i < NUM_SERVOS; i++) {
      pca.setPWM(SERVO_CHANNELS[i], 0, pos);
    }
    delay(15);
  }

  delay(2000);

  // Rotate all servos back to minimum position
  for (int pos = SERVO_MAX_PULSE; pos >= SERVO_MIN_PULSE; pos -= 1) {
    for (int i = 0; i < NUM_SERVOS; i++) {
      pca.setPWM(SERVO_CHANNELS[i], 0, pos);
    }
    delay(15);
  }

  delay(2000);
}