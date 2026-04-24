#include "buzzer.hpp"
#include "app_state.hpp"
#include "dfns_consts_libs.hpp"

void buzz_pattern() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER, 1000);
    delay(150);
    noTone(BUZZER);
    delay(100);
  }
}

void handle_buzzer_sound() {
  static unsigned long lastBuzz = 0;
  bool anyActive = false;
  for (int i = 0; i < NUM_PERSONS; i++) {
    if (appState.buzzerActive[i]) {
      anyActive = true;
      break;
    }
  }
  if (anyActive && millis() - lastBuzz > 3000) {
    buzz_pattern();
    lastBuzz = millis();
  }
}