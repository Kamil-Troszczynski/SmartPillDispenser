#include "buttons.hpp"
#include "app_state.hpp"
#include "servos.hpp"
#include "screen.hpp"
#include "schedules.hpp"


void handle_buttons_mcp(Adafruit_MCP23X17 & mcp, Adafruit_PWMServoDriver & pca) {
  unsigned long now = millis();
  static bool lastBtnUp = false;
  static bool lastBtnDown = false;
  static bool lastBtnOk = false;

  bool btnUp = !mcp.digitalRead(MCP_BTN_UP);
  bool btnDown = !mcp.digitalRead(MCP_BTN_DOWN);
  bool btnOk = !mcp.digitalRead(MCP_BTN_OK);

  bool upPressed = btnUp && !lastBtnUp;
  bool downPressed = btnDown && !lastBtnDown;
  bool okPressed = btnOk && !lastBtnOk;

  lastBtnUp = btnUp;
  lastBtnDown = btnDown;
  lastBtnOk = btnOk;

  if (now - appState.lastBtnTime < DEBOUNCE_MS) return;

  if (upPressed) {
    appState.lastBtnTime = now;
    if (appState.selectedIndex > 0) {
      appState.selectedIndex--;
      if (appState.selectedIndex < appState.scrollOffset)
        appState.scrollOffset--;
      draw_ui();
    }
  }
  else if (downPressed) {
    appState.lastBtnTime = now;
    if (appState.selectedIndex < NUM_PERSONS - 1) {
      appState.selectedIndex++;
      if (appState.selectedIndex >= appState.scrollOffset + VISIBLE_ROWS)
        appState.scrollOffset++;
      draw_ui();
    }
  }
  else if (okPressed) {
    appState.lastBtnTime = now;
    int idx = appState.selectedIndex;
    if (is_in_window(idx)) {
      appState.buzzerActive[idx] = false;
      appState.buzzerAcked[idx]  = true;
      move_servo(persons[idx].servoIndex, pca);
      Serial.printf("Lek podany: %s\n", persons[idx].name);
    } else {
      Serial.printf("%s: poza oknem harmonogramu\n", persons[idx].name);
    }
    draw_ui();
  }
}
