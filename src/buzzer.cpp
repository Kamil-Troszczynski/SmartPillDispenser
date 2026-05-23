#include "buzzer.hpp"
#include "app_state.hpp"
#include "dfns_consts_libs.hpp"


struct BuzzerState {
  bool isActive = false;
  unsigned long startTime = 0;
  unsigned long lastToggle = 0;
  int cycleCount = 0;
  bool pinState = false;
  const unsigned long CYCLE_DURATION = 2000;      // ms - ile długo buzzerac
  const unsigned long HIGH_TIME = 250;            // µs - gdy HIGH
  const unsigned long LOW_TIME = 107;             // µs - gdy LOW
  const unsigned long INTER_CYCLE_DELAY = 50;     // ms - między cyklami
} buzzerState;


void buzzer_start(unsigned long durationMs, Adafruit_MCP23X17 & mcp) {
  buzzerState.isActive = true;
  buzzerState.startTime = millis();
  buzzerState.lastToggle = micros();
  buzzerState.cycleCount = 0;
  buzzerState.pinState = false;
  mcp.digitalWrite(MCP_BUZZER, LOW);
}


void buzzer_update(Adafruit_MCP23X17 & mcp) {
  if (!buzzerState.isActive) return;
  
  unsigned long elapsed = millis() - buzzerState.startTime;

  if (elapsed > 5 * buzzerState.CYCLE_DURATION) {
    buzzerState.isActive = false;
    mcp.digitalWrite(MCP_BUZZER, LOW);
    return;
  }
  
  unsigned long microsElapsed = micros() - buzzerState.lastToggle;
  unsigned long targetTime = buzzerState.pinState ? 
                             buzzerState.HIGH_TIME : 
                             buzzerState.LOW_TIME;
  
  if (microsElapsed >= targetTime) {
    buzzerState.pinState = !buzzerState.pinState;
    mcp.digitalWrite(MCP_BUZZER, buzzerState.pinState ? HIGH : LOW);
    buzzerState.lastToggle = micros();
  }
}


void buzz_pattern_mcp(Adafruit_MCP23X17 & mcp) {
  buzzer_start(5000, mcp);
}

void handle_buzzer_sound_mcp(Adafruit_MCP23X17 & mcp) {
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