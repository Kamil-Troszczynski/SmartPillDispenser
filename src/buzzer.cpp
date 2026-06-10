#include "buzzer.hpp"
#include "app_state.hpp"
#include "persons.hpp"

struct BuzzerState {
    bool isActive = false;
    unsigned long startTime = 0;
    unsigned long lastToggle = 0;
    bool pinState = false;
    
    const unsigned long CYCLE_DURATION = 1000;
    const unsigned long HIGH_TIME = 500;
} buzzerState;

void buzzer_start(unsigned long durationMs, Adafruit_MCP23X17& mcp) {
    (void)durationMs;
    
    buzzerState.isActive = true;
    buzzerState.startTime = millis();
    buzzerState.lastToggle = millis();
    buzzerState.pinState = true;      
    mcp.digitalWrite(MCP_BUZZER, HIGH); 
}

void buzzer_update(Adafruit_MCP23X17& mcp) {
    if (!buzzerState.isActive) return;
    if (millis() - buzzerState.startTime > 5 * buzzerState.CYCLE_DURATION) {
        buzzerState.isActive = false;
        mcp.digitalWrite(MCP_BUZZER, LOW);
        return;
    }

    unsigned long elapsed = millis() - buzzerState.lastToggle;
    
    if (buzzerState.pinState) {
        if (elapsed >= buzzerState.HIGH_TIME) {
            buzzerState.pinState = false;
            mcp.digitalWrite(MCP_BUZZER, LOW);
            buzzerState.lastToggle = millis();
        }
    } else {
        if (elapsed >= (buzzerState.CYCLE_DURATION - buzzerState.HIGH_TIME)) {
            buzzerState.pinState = true;
            mcp.digitalWrite(MCP_BUZZER, HIGH);
            buzzerState.lastToggle = millis();
        }
    }
}

void buzz_pattern_mcp(Adafruit_MCP23X17& mcp) {
    buzzer_start(5000, mcp); // Odpal 5-sekundową sekwencję
}

void handle_buzzer_sound_mcp(Adafruit_MCP23X17& mcp) {
    static unsigned long lastBuzz = 0;
    bool anyActive = false;
    
    for (int i = 0; i < NUM_PERSONS; i++) {
        if (appState.buzzerActive[i]) {
            anyActive = true;
            break;
        }
    }

    if (anyActive && millis() - lastBuzz > 6000) {
        buzz_pattern_mcp(mcp);
        lastBuzz = millis();
    }
}