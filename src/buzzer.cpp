#include "buzzer.hpp"
#include "app_state.hpp"
#include "persons.hpp"

struct BuzzerState {
    bool isActive = false;
    unsigned long startTime = 0;
    unsigned long lastToggle = 0;
    bool pinState = false;
    const unsigned long CYCLE_DURATION = 2000;
    const unsigned long HIGH_TIME = 250;
    const unsigned long LOW_TIME = 107;
} buzzerState;

void buzzer_start(unsigned long durationMs, Adafruit_MCP23X17& mcp) {
    (void)durationMs;
    buzzerState.isActive = true;
    buzzerState.startTime = millis();
    buzzerState.lastToggle = micros();
    buzzerState.pinState = false;
    mcp.digitalWrite(MCP_BUZZER, LOW);
}

void buzzer_update(Adafruit_MCP23X17& mcp) {
    if (!buzzerState.isActive) return;

    if (millis() - buzzerState.startTime > 5 * buzzerState.CYCLE_DURATION) {
        buzzerState.isActive = false;
        mcp.digitalWrite(MCP_BUZZER, LOW);
        return;
    }

    unsigned long elapsed = micros() - buzzerState.lastToggle;
    unsigned long target = buzzerState.pinState ? buzzerState.HIGH_TIME : buzzerState.LOW_TIME;
    if (elapsed >= target) {
        buzzerState.pinState = !buzzerState.pinState;
        mcp.digitalWrite(MCP_BUZZER, buzzerState.pinState ? HIGH : LOW);
        buzzerState.lastToggle = micros();
    }
}

void buzz_pattern_mcp(Adafruit_MCP23X17& mcp) {
    buzzer_start(5000, mcp);
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
    if (anyActive && millis() - lastBuzz > 3000) {
        buzz_pattern_mcp(mcp);
        lastBuzz = millis();
    }
}
