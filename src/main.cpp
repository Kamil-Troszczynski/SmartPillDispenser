// #include "app_state.hpp"
// #include "buzzer.hpp"
// #include "screen.hpp"
// #include "servos.hpp"
// #include "time.hpp"
// #include "schedules.hpp"
// #include "persons.hpp"
// #include "buttons.hpp"
// #include "dfns_consts_libs.hpp"


// AppState appState;
// TFT_eSPI tft = TFT_eSPI();
// ESP32Time rtc(3600);


// Adafruit_PWMServoDriver pca;
// Adafruit_MCP23X17 mcp;


// const int NUM_SENSORS = 4;
// const int SENSOR_PINS[NUM_SENSORS]    = { 13, 12, 14, 27 };
// const char* SENSOR_NAMES[NUM_SENSORS] = { "Sensor 1", "Sensor 2", "Sensor 3", "Sensor 4" };
// int lastSensorState[NUM_SENSORS]      = { 0, 0, 0, 0 };


// void setup_sensors() {
//   for (int i = 0; i < NUM_SENSORS; i++) {
//     pinMode(SENSOR_PINS[i], INPUT);
//     digitalWrite(SENSOR_PINS[i], HIGH);
//     lastSensorState[i] = digitalRead(SENSOR_PINS[i]);
//   }
//   Serial.println("Fotocells system is ready");
// }


// void handle_sensors() {
//   for (int i = 0; i < NUM_SENSORS; i++) {
//     int state = digitalRead(SENSOR_PINS[i]);
//     if (!state && lastSensorState[i]) {

//       Serial.printf("[%s] Signal interrupted!\n", SENSOR_NAMES[i]);
      
//       // Czujnik i mapuje się na osobę i
//       if (i < NUM_PERSONS && appState.waitingForSensor[i]) {
//         appState.waitingForSensor[i] = false;
//         appState.doseDelivered[i] = true;
//         Serial.printf("Dose delivered for %s\n", persons[i].name);
//         draw_ui();
//       } else if (i < NUM_PERSONS) {
//         Serial.printf("Sensor %d triggered but no one waiting\n", i);
//       }
//     }

//     if (state && !lastSensorState[i]) {
//       Serial.printf("[%s] Signal restored\n", SENSOR_NAMES[i]);
//     }

//     lastSensorState[i] = state;
//   }
// }


// void setup() {
//   Serial.begin(115200);

//   tft.init();
//   tft.setRotation(1);
//   tft.fillScreen(C_BG);
//   draw_ui();

//   Wire.begin(21, 22);

//   if (!mcp.begin_I2C(0x20)) {
//     Serial.println("Błąd: MCP23017 nie znaleziony!");
//     while (1);
//   }

//   mcp.pinMode(MCP_BTN_UP,   INPUT_PULLUP);
//   mcp.pinMode(MCP_BTN_DOWN, INPUT_PULLUP);
//   mcp.pinMode(MCP_BTN_OK,   INPUT_PULLUP);
//   mcp.pinMode(MCP_BUZZER,   OUTPUT);
//   mcp.digitalWrite(MCP_BUZZER, LOW);

//   setup_up_servos(pca);
//   setup_sensors();

//   sync_time_from_NTP();
//   draw_ui();
// }

// void loop() {
//   handle_buttons_mcp(mcp, pca);

//   if (check_schedules()) {
//     draw_ui();
//   }

//   buzzer_update(mcp);
//   handle_buzzer_sound_mcp(mcp);

//   handle_sensors();

//   static unsigned long lastRefresh = 0;
//   if (millis() - lastRefresh > 10000) {
//     lastRefresh = millis();
//     draw_ui();
//   }
// }

#include "mqtt_broker.hpp"
#include "time.hpp"
#include "screen.hpp"
#include "buttons.hpp"
#include "servos.hpp"
#include "schedules.hpp"
#include "power.hpp"

#define BROKER_HOST "192.168.161.76"
#define BROKER_PORT 1883
#define TOPIC_SYNC  "pill_dispenser/1/sync"
#define TOPIC_CONF  "pill_dispenser/1/pub_confirmation"

Adafruit_MCP23X17 mcp;

AppState appState;
TFT_eSPI tft = TFT_eSPI();
ESP32Time rtc(3600);


void setup() {
    Serial.begin(115200);
    delay(1000);
    Wire.begin(21, 22);

    if (!mcp.begin_I2C(0x20)) {
      Serial.println("Błąd: MCP23017 nie znaleziony!");
      while (1);
    }

    mcp.pinMode(MCP_BTN_UP,   INPUT_PULLUP);
    mcp.pinMode(MCP_BTN_DOWN, INPUT_PULLUP);
    mcp.pinMode(MCP_BTN_OK,   INPUT_PULLUP);
    buttons_init_mcp(mcp);

    Serial.printf("Stan poczatkowy: UP=%d DOWN=%d OK=%d\n",
    mcp.digitalRead(MCP_BTN_UP),
    mcp.digitalRead(MCP_BTN_DOWN),
    mcp.digitalRead(MCP_BTN_OK));

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(C_BG);

    sync_time_from_NTP();

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    draw_sync_status("Laczenie z WiFi...", "MQTT", C_HEADER_BG, C_HEADER_TXT);
    int att = 0;
    while (WiFi.status() != WL_CONNECTED && att < 20) { delay(500); att++; }

    if (WiFi.status() == WL_CONNECTED) {
        mqtt_init(BROKER_HOST, BROKER_PORT, TOPIC_SYNC, TOPIC_CONF);
        unsigned long t = millis();
        while (!syncReceived && millis() - t < 5000) {
            mqtt_loop();
            delay(50);
        }
        Serial.printf("syncReceived=%d NUM_PERSONS=%d\n", syncReceived, NUM_PERSONS);
        power_init(mcp);
    }

    draw_ui();
}

void loop() {
    mqtt_loop();

    if (power_should_poll_buttons())
        handle_buttons_mcp(mcp);

    if (check_schedules()) draw_ui();

    power_tick(mcp);
}