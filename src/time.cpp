#include "time.hpp"
#include "app_state.hpp"
#include "dfns_consts_libs.hpp"
#include "screen.hpp"


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;


void sync_time_from_NTP() {
  WiFi.mode(WIFI_STA);
  draw_sync_status("Laczenie z WiFi...", "Prosze czekac", C_HEADER_BG, C_HEADER_TXT);

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    draw_sync_status("Laczenie z WiFi...", "Synchronizacja czasu", C_HEADER_BG, C_HEADER_TXT);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    int retries = 0;
    bool timeReady = getLocalTime(&timeinfo);
    while (!timeReady && retries < 10) {
      delay(500);
      retries++;
      timeReady = getLocalTime(&timeinfo);
    }
    if (timeReady) {
      rtc.setTimeStruct(timeinfo);
      draw_sync_status("Czas ustawiony", "OK", C_ACTIVE_BG, C_ACTIVE_TXT);
      delay(800);
    }
  } else {
    draw_sync_status("Brak WiFi!", "Uzywam lokalnego czasu", C_WARN_BG, C_WARN_TXT);
    delay(1500);
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
