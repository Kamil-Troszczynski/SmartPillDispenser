#include "schedules.hpp"
#include "app_state.hpp"
#include "screen.hpp"


int to_minutes(int h, int m) {
  return h * 60 + m;
}


bool is_in_window(int personIdx) {
  int now = to_minutes(rtc.getHour(true), rtc.getMinute());
  int start = to_minutes(persons[personIdx].buzzerStartHour, persons[personIdx].buzzerStartMin);
  int end = to_minutes(persons[personIdx].buzzerEndHour, persons[personIdx].buzzerEndMin);
  return (now >= start && now < end);
}


bool check_schedules() {
  static int lastCheckedMinute = -1;
  int currentMinute = rtc.getHour(true) * 60 + rtc.getMinute();
  if (currentMinute == lastCheckedMinute) return false;
  lastCheckedMinute = currentMinute;

  bool changed = false;
  for (int i = 0; i < NUM_PERSONS; i++) {
    bool inWindow = is_in_window(i);
    bool shouldBeActive = inWindow && !appState.buzzerAcked[i];

    if (appState.buzzerActive[i] != shouldBeActive) {
      appState.buzzerActive[i] = shouldBeActive;
      changed = true;
    }

    if (!inWindow && appState.buzzerAcked[i]) {
      appState.buzzerAcked[i] = false;
      changed = true;
    }

    if (!inWindow) {
      appState.doseDelivered[i] = false;
    }
  }

  return changed;
}
