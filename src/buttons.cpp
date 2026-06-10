#include "buttons.hpp"
#include "app_state.hpp"
#include "servos.hpp"
#include "screen.hpp"
#include "schedules.hpp"
#include "mqtt_broker.hpp"
#include "power.hpp"
#include "persons.hpp"

void buttons_init_mcp(Adafruit_MCP23X17& mcp) {
    pinMode(MCP_INT_PIN, INPUT_PULLUP);
    mcp.setupInterrupts(true, false, LOW);
    mcp.setupInterruptPin(MCP_BTN_UP,   CHANGE);
    mcp.setupInterruptPin(MCP_BTN_DOWN, CHANGE);
    mcp.setupInterruptPin(MCP_BTN_OK,   CHANGE);
    mcp.clearInterrupts();
}

void buttons_after_wake(Adafruit_MCP23X17& mcp) {
    mcp.clearInterrupts();
}
void handle_buttons_mcp(Adafruit_MCP23X17& mcp, Adafruit_PWMServoDriver& pca) {
    static bool initialized = false;
    static bool lastBtnUp   = false;
    static bool lastBtnDown = false;
    static bool lastBtnOk   = false;

    bool btnUp   = mcp.digitalRead(MCP_BTN_UP)   == 0;
    bool btnDown = mcp.digitalRead(MCP_BTN_DOWN) == 0;
    bool btnOk   = mcp.digitalRead(MCP_BTN_OK)   == 0;

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

    if (upPressed || downPressed || okPressed)
        power_notify_user_activity();

    if (upPressed) {
        appState.lastBtnTime = now;
        if (appState.detailView) {
            if (appState.detailSelectedIndex > 0) {
                appState.detailSelectedIndex--;
                sync_detail_scroll(persons[appState.selectedIndex].numEvents);
            }
        } else if (appState.selectedIndex > 0) {
            appState.selectedIndex--;
            if (appState.selectedIndex < appState.scrollOffset)
                appState.scrollOffset--;
        }
        draw_ui();
    }
    else if (downPressed) {
        appState.lastBtnTime = now;
        if (appState.detailView) {
            int maxIdx = persons[appState.selectedIndex].numEvents + 1;
            if (appState.detailSelectedIndex < maxIdx) {
                appState.detailSelectedIndex++;
                sync_detail_scroll(persons[appState.selectedIndex].numEvents);
            }
        } else if (appState.selectedIndex < NUM_PERSONS - 1) {
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
            Person& p = persons[idx];
            int sel = appState.detailSelectedIndex;

            if (sel < p.numEvents) {
                p.events[sel].checked = !p.events[sel].checked;
            } else if (sel == p.numEvents) {
                if (!is_in_dose_window(idx)) {
                    Serial.printf("%s: poza oknem harmonogramu\n", p.name);
                } else {
                    bool anyChecked = false;
                    for (int e = 0; e < p.numEvents; e++) {
                        if (p.events[e].checked) anyChecked = true;
                    }
                    if (anyChecked) {
                        appState.buzzerActive[idx]     = false;
                        appState.buzzerAcked[idx]      = true;
                        appState.doseDelivered[idx]    = false;
                        appState.waitingForSensor[idx] = true;
                        
                        // Przechodzimy przez wszystkie leki i wyrzucamy te zaznaczone
                        for (int e = 0; e < p.numEvents; e++) {
                            if (p.events[e].checked) {
                                mqtt_publish_confirmation(p.events[e].dts_id, p.events[e].dosage);
                                dispense_servo(pca, idx, e);
                                p.events[e].checked = false;
                            }
                        }
                    }
                }
            } else if (sel == p.numEvents + 1) {
                appState.detailView = false;
            }
        } else {
            appState.detailView = true;
            appState.detailSelectedIndex = 0;
            appState.detailScrollOffset = 0;
        }
        draw_ui();
    }
}