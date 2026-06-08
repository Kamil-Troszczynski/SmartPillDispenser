#pragma once

#include "dfns_consts_libs.hpp"
#include "persons.hpp"


struct AppState {
    int  selectedIndex = 0;
    int detailSelectedIndex = 0;
    int  scrollOffset  = 0;
    bool detailView    = false;
    bool buzzerActive[MAX_PERSONS]     = {false};
    bool buzzerAcked[MAX_PERSONS]      = {false};
    bool doseDelivered[MAX_PERSONS]    = {false};
    bool waitingForSensor[MAX_PERSONS] = {false};
    unsigned long lastBtnTime = 0;
};


extern AppState appState;
extern TFT_eSPI tft;
extern ESP32Time rtc;
