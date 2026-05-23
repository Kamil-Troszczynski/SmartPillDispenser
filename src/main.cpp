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


void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);

  tft.fillScreen(C_BG);
  draw_ui();

  Wire.begin(21, 22);

  // MCP23017 validation
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

  static unsigned long lastRefresh = 0;
  if (millis() - lastRefresh > 10000) {
    lastRefresh = millis();
    draw_ui();
  }
}
