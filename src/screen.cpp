#include "screen.hpp"
#include "app_state.hpp"
#include "persons.hpp"
#include "schedules.hpp"


void draw_header() {
  tft.fillRect(0, 0, SCREEN_W, HEADER_H, C_HEADER_BG);

  tft.setTextColor(C_HEADER_TXT, C_HEADER_BG);
  tft.setTextSize(1);
  tft.setCursor(6, 5);
  tft.print("AKESO");

  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", rtc.getHour(true), rtc.getMinute());
  tft.setCursor(SCREEN_W - 34, 5);
  tft.print(buf);

  tft.drawFastHLine(0, HEADER_H, SCREEN_W, C_SEP);
}


void draw_scrollbar(int scrollOffset) {
  if (NUM_PERSONS <= VISIBLE_ROWS) return;
  const int bar_x = SCREEN_W - 4;
  const int bar_y = LIST_Y;
  const int bar_h = VISIBLE_ROWS * ROW_H;
  int thumb_h = max(8, bar_h * VISIBLE_ROWS / NUM_PERSONS);
  int thumb_y = bar_y + (bar_h - thumb_h) * scrollOffset / max(1, NUM_PERSONS - VISIBLE_ROWS);

  tft.fillRect(bar_x, bar_y, 4, bar_h, C_SCROLLBG);
  tft.fillRect(bar_x, thumb_y, 4, thumb_h, C_SCROLLFG);
}


void draw_person_row(int personIdx, int rowY, bool isSelected) {
  Person& p = persons[personIdx];
  int now = rtc.getHour(true) * 60 + rtc.getMinute();
  int start = p.buzzerStartHour * 60 + p.buzzerStartMin;
  int end = p.buzzerEndHour * 60 + p.buzzerEndMin;
  bool inWindow = (now >= start && now < end);
  bool buzzing = appState.buzzerActive[personIdx];

  uint16_t rowBg = (personIdx % 2 == 0) ? C_ROW_EVEN : C_ROW_ODD;
  if (isSelected) rowBg = C_SELECT_BG;
  tft.fillRect(0, rowY, SCREEN_W, ROW_H, rowBg);

  if (isSelected) {
    tft.fillRect(0, rowY, 3, ROW_H, C_SELECT_ACC);
  }

  tft.setTextSize(1);
  tft.setTextColor(isSelected ? C_SELECT_ACC : C_TEXT, rowBg);
  tft.setCursor(7, rowY + 5);
  tft.print(p.name);

  char schedule[12];
  snprintf(schedule, sizeof(schedule), "%02d:%02d-%02d:%02d",
           p.buzzerStartHour, p.buzzerStartMin,
           p.buzzerEndHour, p.buzzerEndMin);
  tft.setTextColor(C_MUTED, rowBg);
  tft.setCursor(7, rowY + 18);
  tft.print(schedule);

  if (buzzing) {
    tft.fillRoundRect(SCREEN_W - 46, rowY + 8, 38, 14, 4, C_WARN_BG);
    tft.setTextColor(C_WARN_TXT, C_WARN_BG);
    tft.setCursor(SCREEN_W - 40, rowY + 12);
    tft.print("PORA!");
  } else if (inWindow) {
    tft.fillRoundRect(SCREEN_W - 46, rowY + 8, 38, 14, 4, C_ACTIVE_BG);
    tft.setTextColor(C_ACTIVE_TXT, C_ACTIVE_BG);
    tft.setCursor(SCREEN_W - 34, rowY + 12);
    tft.print("OK");
  }
  tft.drawFastHLine(0, rowY + ROW_H - 1, SCREEN_W, C_SEP);
}


void draw_footer() {
  tft.fillRect(0, FOOTER_Y, SCREEN_W, FOOTER_H, C_FOOTER_BG);
  tft.drawFastHLine(0, FOOTER_Y, SCREEN_W, C_SEP);
  tft.setTextColor(C_FOOTER_TXT, C_FOOTER_BG);
  tft.setTextSize(1);
  tft.setCursor(4, FOOTER_Y + 2);
  tft.print("UP/DN-wybor  OK-podaj lek");
}


void draw_ui() {
  draw_header();
  for (int i = 0; i < VISIBLE_ROWS; i++) {
    int personIdx = appState.scrollOffset + i;
    if (personIdx >= NUM_PERSONS) {
      tft.fillRect(0, LIST_Y + i * ROW_H, SCREEN_W - 4, ROW_H, C_BG);
      continue;
    }
    draw_person_row(personIdx, 
                    LIST_Y + i * ROW_H, 
                    personIdx == appState.selectedIndex);
  }
  draw_scrollbar(appState.scrollOffset);
  draw_footer();
}
