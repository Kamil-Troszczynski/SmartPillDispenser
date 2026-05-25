#pragma once

#include "dfns_consts_libs.hpp"
#include "persons.hpp"


struct AppState {
  int selectedIndex = 0;
  int scrollOffset = 0;
  bool buzzerActive[NUM_PERSONS] = {false};
  bool buzzerAcked[NUM_PERSONS] = {false};
  bool doseDelivered[NUM_PERSONS] = {false};
  bool waitingForSensor[NUM_PERSONS] = {false};
  unsigned long lastBtnTime = 0;
};


extern AppState appState;
extern TFT_eSPI tft;
extern ESP32Time rtc;
