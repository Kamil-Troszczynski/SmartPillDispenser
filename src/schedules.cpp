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


bool is_in_dose_window(int personIdx) {
    if (personIdx < 0 || personIdx >= NUM_PERSONS) return false;
    int nowMin = rtc.getHour(true) * 60 + rtc.getMinute();
    Person& p = persons[personIdx];
    for (int e = 0; e < p.numEvents; e++) {
        int hh = 0, mm = 0, ss = 0;
        sscanf(p.events[e].time, "%d:%d:%d", &hh, &mm, &ss);
        int evMin = hh * 60 + mm;
        if (nowMin >= evMin && nowMin < evMin + 5) return true;
    }
    return false;
}


bool check_schedules() {
  static int lastCheckedMinute = -1;
  int currentMinute = rtc.getHour(true) * 60 + rtc.getMinute();
  if (currentMinute == lastCheckedMinute) return false;
  lastCheckedMinute = currentMinute;

  bool changed = false;
  for (int i = 0; i < NUM_PERSONS; i++) {
    bool inWindow = is_in_dose_window(i);
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


static uint64_t us_until_next_minute() {
    int sec = rtc.getSecond();
    return (uint64_t)(60 - sec) * 1000000ULL;
}


uint64_t us_until_next_schedule_wake() {
    const uint64_t maxSleep = 60 * 1000000ULL;
    const uint64_t minSleep = 100000ULL;

    uint64_t sleepUs = us_until_next_minute();

    int nowMin = rtc.getHour(true) * 60 + rtc.getMinute();
    int nowSec = rtc.getSecond();
    int bestDeltaMin = -1;

    for (int i = 0; i < NUM_PERSONS; i++) {
        Person& p = persons[i];
        for (int e = 0; e < p.numEvents; e++) {
            int hh = 0, mm = 0, ss = 0;
            sscanf(p.events[e].time, "%d:%d:%d", &hh, &mm, &ss);
            int evMin = hh * 60 + mm;
            int deltaMin = evMin - nowMin;
            if (deltaMin < 0) continue;
            if (bestDeltaMin < 0 || deltaMin < bestDeltaMin)
                bestDeltaMin = deltaMin;
        }
    }

    if (bestDeltaMin >= 0) {
        int deltaSec = bestDeltaMin * 60 - nowSec;
        if (deltaSec < 1) deltaSec = 1;
        uint64_t doseUs = (uint64_t)deltaSec * 1000000ULL;
        if (doseUs < sleepUs) sleepUs = doseUs;
    }

    if (sleepUs > maxSleep) sleepUs = maxSleep;
    if (sleepUs < minSleep) sleepUs = minSleep;
    return sleepUs;
}
