#include "time.hpp"
#include "app_state.hpp"
#include "dfns_consts_libs.hpp"


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;


void sync_time_from_NTP() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_TEXT, C_BG);
  tft.setTextSize(1);
  tft.setCursor(10, 20);
  tft.print("Laczenie z WiFi...");

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    tft.setCursor(10, 36);
    tft.print("Synchronizacja czasu");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    int retries = 0;
    while (!getLocalTime(&timeinfo) && retries < 10) {
      delay(500);
      retries++;
    }
    if (getLocalTime(&timeinfo)) {
      rtc.setTimeStruct(timeinfo);
      tft.setTextColor(C_ACTIVE_BG, C_BG);
      tft.setCursor(10, 52);
      tft.print("OK");
      delay(800);
    }
  } else {
    tft.setCursor(10, 36);
    tft.setTextColor(TFT_RED, C_BG);
    tft.print("Brak WiFi!");
    delay(1500);
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}