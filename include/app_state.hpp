#pragma once

#include "dfns_consts_libs.hpp"
#include "persons.hpp"


struct AppState {
  // UI navigation
  int selectedIndex = 0;
  int scrollOffset = 0;

  // Buzzer state
  bool buzzerActive[NUM_PERSONS] = {false};
  bool buzzerAcked[NUM_PERSONS] = {false};

  // Debouncing
  unsigned long lastBtnTime = 0;
};

// Global app state instance
extern AppState appState;
extern TFT_eSPI tft;
extern ESP32Time rtc;
