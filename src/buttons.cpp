#include "buttons.hpp"
#include "app_state.hpp"
#include "servos.hpp"
#include "screen.hpp"
#include "schedules.hpp"


// void handle_buttons_mcp(Adafruit_MCP23X17 & mcp/*, Adafruit_PWMServoDriver & pca*/) {
//   unsigned long now = millis();
//   static bool lastBtnUp = false;
//   static bool lastBtnDown = false;
//   static bool lastBtnOk = false;

//   bool btnUp = !mcp.digitalRead(MCP_BTN_UP);
//   bool btnDown = !mcp.digitalRead(MCP_BTN_DOWN);
//   bool btnOk = !mcp.digitalRead(MCP_BTN_OK);

//   bool upPressed = btnUp && !lastBtnUp;
//   bool downPressed = btnDown && !lastBtnDown;
//   bool okPressed = btnOk && !lastBtnOk;

//   lastBtnUp = btnUp;
//   lastBtnDown = btnDown;
//   lastBtnOk = btnOk;

//   if (now - appState.lastBtnTime < DEBOUNCE_MS) return;

//   if (upPressed) {
//     appState.lastBtnTime = now;
//     if (appState.selectedIndex > 0) {
//       appState.selectedIndex--;
//       if (appState.selectedIndex < appState.scrollOffset)
//         appState.scrollOffset--;
//       draw_ui();
//     }
//   }
//   else if (downPressed) {
//     appState.lastBtnTime = now;
//     if (appState.selectedIndex < NUM_PERSONS - 1) {
//       appState.selectedIndex++;
//       if (appState.selectedIndex >= appState.scrollOffset + VISIBLE_ROWS)
//         appState.scrollOffset++;
//       draw_ui();
//     }
//   }
//   else if (okPressed) {
//     appState.lastBtnTime = now;
//     int idx = appState.selectedIndex;
//     if (is_in_window(idx)) {
//         appState.buzzerActive[idx] = false;
//         appState.buzzerAcked[idx]  = true;
//         appState.doseDelivered[idx] = false;
//         // move_servo(persons[idx].servoIndex, pca);
//         // znajdź aktywne zdarzenie i wyślij potwierdzenie
//         // int nowMin = rtc.getHour(true) * 60 + rtc.getMinute();
//         // for (int e = 0; e < persons[idx].numEvents; e++) {
//         //     int hh, mm, ss;
//         //     sscanf(persons[idx].events[e].time, "%d:%d:%d", &hh, &mm, &ss);
//         //     int evMin = hh * 60 + mm;
//         //     if (abs(nowMin - evMin) <= 5) {
//         //         mqtt_publish_confirmation(persons[idx].events[e].dts_id,
//         //                                   persons[idx].events[e].dosage);
//         //         break;
//         //     }
//         // }
//         appState.waitingForSensor[idx] = true;
//         Serial.printf("Wyrzut tabletki: %s\n", persons[idx].name);

//     } else {
//       Serial.printf("%s: poza oknem harmonogramu\n", persons[idx].name);
//     }
//     draw_ui();
//   }
// }


// void handle_buttons_mcp(Adafruit_MCP23X17 & mcp) {
//     unsigned long now = millis();
//     static bool lastBtnUp   = false;
//     static bool lastBtnDown = false;
//     static bool lastBtnOk   = false;

//     if (now - appState.lastBtnTime < DEBOUNCE_MS) return;

//     bool btnUp   = mcp.digitalRead(MCP_BTN_UP);
//     bool btnDown = mcp.digitalRead(MCP_BTN_DOWN);
//     bool btnOk   = mcp.digitalRead(MCP_BTN_OK);

//     bool upPressed   = btnUp   && !lastBtnUp;
//     bool downPressed = btnDown && !lastBtnDown;
//     bool okPressed   = btnOk   && !lastBtnOk;

//     lastBtnUp   = btnUp;
//     lastBtnDown = btnDown;
//     lastBtnOk   = btnOk;

//     // if (upPressed) {
//     // appState.lastBtnTime = now;
//     // if (appState.selectedIndex > 0) {
//     //     appState.selectedIndex--;
//     //     if (appState.selectedIndex < appState.scrollOffset)
//     //         appState.scrollOffset--;
//     //     draw_ui();
//     // }
//     // }
//     if (downPressed) {
//         appState.lastBtnTime = now; 
//         if (appState.selectedIndex < NUM_PERSONS - 1) {
//             appState.selectedIndex++;
//             if (appState.selectedIndex >= appState.scrollOffset + VISIBLE_ROWS)
//                 appState.scrollOffset++;
//             draw_ui();
//         }
//     }
//     else if (okPressed) {
//     appState.lastBtnTime = now;

//     if (appState.detailView) {
//         int idx = appState.selectedIndex;
//         if (is_in_window(idx)) {
//             appState.buzzerActive[idx]     = false;
//             appState.buzzerAcked[idx]      = true;
//             appState.doseDelivered[idx]    = false;
//             appState.waitingForSensor[idx] = true;
//             Serial.printf("Wyrzut tabletki: %s\n", persons[idx].name);
//         } else {
//             Serial.printf("%s: poza oknem harmonogramu\n", persons[idx].name);
//         }
//     } else {
//         appState.detailView = true;
//     }
//       draw_ui();
//     }
//     else if (upPressed) {
//         appState.lastBtnTime = now;
//         if (appState.detailView) {
//             // UP w szczegółach cofa do listy
//             appState.detailView = false;
//         } else if (appState.selectedIndex > 0) {
//             appState.selectedIndex--;
//             if (appState.selectedIndex < appState.scrollOffset)
//                 appState.scrollOffset--;
//         }
//         draw_ui();
//     }
// }
void handle_buttons_mcp(Adafruit_MCP23X17& mcp) {
    static bool initialized = false;
    static bool lastBtnUp   = false;
    static bool lastBtnDown = false;
    static bool lastBtnOk   = false;

    // Wciśnięty = 0 (LOW), nieprzciśnięty = 1 (HIGH)
    bool btnUp   = mcp.digitalRead(MCP_BTN_UP)   == 0;
    bool btnDown = mcp.digitalRead(MCP_BTN_DOWN) == 0;
    bool btnOk   = mcp.digitalRead(MCP_BTN_OK)   == 0;

    // Pierwsze wywołanie – zapisz stan bez wykrywania zboczy
    if (!initialized) {
        lastBtnUp   = btnUp;
        lastBtnDown = btnDown;
        lastBtnOk   = btnOk;
        initialized = true;
        return;
    }

    bool upPressed   = btnUp   && !lastBtnUp;
    bool downPressed = btnDown && !lastBtnDown;
    bool okPressed   = btnOk   && !lastBtnOk;

    lastBtnUp   = btnUp;
    lastBtnDown = btnDown;
    lastBtnOk   = btnOk;

    unsigned long now = millis();
    if (now - appState.lastBtnTime < DEBOUNCE_MS) return;

    if (upPressed) {
        appState.lastBtnTime = now;
        if (appState.detailView) {
            appState.detailView = false;
        } else if (appState.selectedIndex > 0) {
            appState.selectedIndex--;
            if (appState.selectedIndex < appState.scrollOffset)
                appState.scrollOffset--;
        }
        draw_ui();
    }
    else if (downPressed) {
        appState.lastBtnTime = now;
        if (appState.selectedIndex < NUM_PERSONS - 1) {
            appState.selectedIndex++;
            if (appState.selectedIndex >= appState.scrollOffset + VISIBLE_ROWS)
                appState.scrollOffset++;
        }
        draw_ui();
    }
    else if (okPressed) {
        appState.lastBtnTime = now;
        if (appState.detailView) {
            int idx = appState.selectedIndex;
            if (is_in_window(idx)) {
                appState.buzzerActive[idx]     = false;
                appState.buzzerAcked[idx]      = true;
                appState.doseDelivered[idx]    = false;
                appState.waitingForSensor[idx] = true;
                Serial.printf("Wyrzut tabletki: %s\n", persons[idx].name);
            } else {
                Serial.printf("%s: poza oknem harmonogramu\n", persons[idx].name);
            }
        } else {
            appState.detailView = true;
        }
        draw_ui();
    }
}