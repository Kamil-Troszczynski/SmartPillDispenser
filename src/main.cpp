#include "app_state.hpp"
#include "buzzer.hpp"
#include "screen.hpp"
#include "servos.hpp"
#include "time.hpp"
#include "schedules.hpp"
#include "persons.hpp"
#include "buttons.hpp"
#include "dfns_consts_libs.hpp"


AppState appState;
TFT_eSPI tft = TFT_eSPI();
ESP32Time rtc(3600);


Adafruit_PWMServoDriver pca;
Adafruit_MCP23X17 mcp;


const int NUM_SENSORS = 1;
const int SENSOR_PINS[NUM_SENSORS]    = { 13 };
const char* SENSOR_NAMES[NUM_SENSORS] = { "Sensor 1" };
int lastSensorState[NUM_SENSORS]      = { 0 };


void setup_sensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(SENSOR_PINS[i], INPUT);
    digitalWrite(SENSOR_PINS[i], HIGH);
    lastSensorState[i] = digitalRead(SENSOR_PINS[i]);
  }
  Serial.println("Fotocells system is ready");
}


void handle_sensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    int state = digitalRead(SENSOR_PINS[i]);
    if (!state && lastSensorState[i]) {

      Serial.printf("[%s] Signal interrupted!\n", SENSOR_NAMES[i]);
      for (int p = 0; p < NUM_PERSONS; p++) {

        if (appState.waitingForSensor[p]) {

          appState.waitingForSensor[p] = false;
          appState.doseDelivered[p] = true;

          Serial.printf("Dose delivered for %s\n", persons[p].name);

          draw_ui();
          break;
        }
      }
    }

    if (state && !lastSensorState[i]) {
      Serial.printf("[%s] Signal restored\n", SENSOR_NAMES[i]);
    }

    lastSensorState[i] = state;
  }
}


void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(C_BG);
  draw_ui();

  Wire.begin(21, 22);

  if (!mcp.begin_I2C(0x20)) {
    Serial.println("Błąd: MCP23017 nie znaleziony!");
    while (1);
  }

  mcp.pinMode(MCP_BTN_UP,   INPUT_PULLUP);
  mcp.pinMode(MCP_BTN_DOWN, INPUT_PULLUP);
  mcp.pinMode(MCP_BTN_OK,   INPUT_PULLUP);
  mcp.pinMode(MCP_BUZZER,   OUTPUT);
  mcp.digitalWrite(MCP_BUZZER, LOW);

  setup_up_servos(pca);
  setup_sensors();

  sync_time_from_NTP();
  draw_ui();
}

void loop() {
  handle_buttons_mcp(mcp, pca);

  if (check_schedules()) {
    draw_ui();
  }

  buzzer_update(mcp);
  handle_buzzer_sound_mcp(mcp);

  handle_sensors();

  static unsigned long lastRefresh = 0;
  if (millis() - lastRefresh > 10000) {
    lastRefresh = millis();
    draw_ui();
  }
}