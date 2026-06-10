#include "mqtt_broker.hpp"
#include "time.hpp"
#include "screen.hpp"
#include "buttons.hpp"
#include "servos.hpp"
#include "schedules.hpp"
#include "power.hpp"
#include "buzzer.hpp"
#include "sensors.hpp"

#define BROKER_HOST "192.168.161.76"
#define BROKER_PORT 1883
#define TOPIC_SYNC  "pill_dispenser/1/sync"
#define TOPIC_CONF  "pill_dispenser/1/pub_confirmation"

Adafruit_MCP23X17 mcp;
Adafruit_PWMServoDriver pca;

AppState appState;
TFT_eSPI tft = TFT_eSPI();
ESP32Time rtc(3600);


void setup() {
    Serial.begin(115200);
    delay(1000);
    Wire.begin(21, 22);

    if (!mcp.begin_I2C(0x20)) {
        Serial.println("Blad: MCP23017 nie znaleziony!");
        while (1);
    }

    mcp.pinMode(MCP_BTN_UP,   INPUT_PULLUP);
    mcp.pinMode(MCP_BTN_DOWN, INPUT_PULLUP);
    mcp.pinMode(MCP_BTN_OK,   INPUT_PULLUP);
    mcp.pinMode(MCP_BUZZER,   OUTPUT);
    mcp.digitalWrite(MCP_BUZZER, LOW);
    buttons_init_mcp(mcp);

    setup_up_servos(pca);
    setup_sensors();

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
        handle_buttons_mcp(mcp, pca);

    if (check_schedules()) draw_ui();

    handle_buzzer_sound_mcp(mcp);
    buzzer_update(mcp);
    handle_sensors();

    power_tick(mcp, pca);
}
