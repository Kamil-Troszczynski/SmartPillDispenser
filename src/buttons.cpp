#include "buttons.hpp"
#include "app_state.hpp"
#include "servos.hpp"
#include "screen.hpp"
#include "schedules.hpp"

extern Servo servos[NUM_SERVOS];

void handle_buttons() {
  unsigned long now = millis();
  if (now - appState.lastBtnTime < DEBOUNCE_MS) return;

  if (digitalRead(BTN_UP) == LOW) {
    appState.lastBtnTime = now;
    if (appState.selectedIndex > 0) {
      appState.selectedIndex--;
      if (appState.selectedIndex < appState.scrollOffset) {
        appState.scrollOffset--;
      }
      draw_ui();
    }
  }
  else if (digitalRead(BTN_DOWN) == LOW) {
    appState.lastBtnTime = now;
    if (appState.selectedIndex < NUM_PERSONS - 1) {
      appState.selectedIndex++;
      if (appState.selectedIndex >= appState.scrollOffset + VISIBLE_ROWS) {
        appState.scrollOffset++;
      }
      draw_ui();
    }
  }
  else if (digitalRead(BTN_OK) == LOW) {
    appState.lastBtnTime = now;
    int idx = appState.selectedIndex;
    if (is_in_window(idx)) {
      appState.buzzerActive[idx] = false;
      appState.buzzerAcked[idx] = true;
      move_servo(persons[idx].servoIndex, servos);
      Serial.printf("Lek podany: %s\n", persons[idx].name);
    } else {
      Serial.printf("%s: poza oknem harmonogramu\n", persons[idx].name);
    }
    draw_ui();
  }
}