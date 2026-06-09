#include "screen.hpp"
#include "app_state.hpp"
#include "persons.hpp"
#include "schedules.hpp"
#include "power.hpp"

static void draw_scrollbar(int scrollOffset) {
    if (NUM_PERSONS <= VISIBLE_ROWS) return;
    const int bar_x = SCREEN_W - 4;
    const int bar_y = LIST_Y;
    const int bar_h = VISIBLE_ROWS * ROW_H;
    int thumb_h = max(8, bar_h * VISIBLE_ROWS / NUM_PERSONS);
    int thumb_y = bar_y + (bar_h - thumb_h) * scrollOffset / max(1, NUM_PERSONS - VISIBLE_ROWS);
    tft.fillRect(bar_x, bar_y, 4, bar_h, C_SCROLLBG);
    tft.fillRect(bar_x, thumb_y, 4, thumb_h, C_SCROLLFG);
}

static void draw_detail_scrollbar(int scrollOffset, int totalItems) {
    if (totalItems <= VISIBLE_DETAIL_ROWS) return;
    const int bar_x = SCREEN_W - 4;
    const int bar_y = DETAIL_LIST_Y;
    const int bar_h = VISIBLE_DETAIL_ROWS * DETAIL_ROW_PITCH;
    int thumb_h = max(6, bar_h * VISIBLE_DETAIL_ROWS / totalItems);
    int thumb_y = bar_y + (bar_h - thumb_h) * scrollOffset / max(1, totalItems - VISIBLE_DETAIL_ROWS);
    tft.fillRect(bar_x, bar_y, 4, bar_h, C_SCROLLBG);
    tft.fillRect(bar_x, thumb_y, 4, thumb_h, C_SCROLLFG);
}

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

void draw_sync_status(const char* line1, const char* line2, uint16_t bg, uint16_t fg) {
    const int boxX = 8;
    const int boxY = HEADER_H + 12;
    const int boxW = SCREEN_W - 16;
    const int boxH = 36;

    tft.fillRoundRect(boxX, boxY, boxW, boxH, 4, bg);
    tft.drawRoundRect(boxX, boxY, boxW, boxH, 4, fg);
    tft.setTextSize(1);
    tft.setTextColor(fg, bg);
    tft.setCursor(boxX + 8, boxY + 9);
    tft.print(line1);
    if (line2 && line2[0])
        tft.setCursor(boxX + 8, boxY + 21), tft.print(line2);
}

void sync_detail_scroll(int numEvents) {
    int sel = appState.detailSelectedIndex;
    if (sel >= numEvents) return;

    if (sel < appState.detailScrollOffset)
        appState.detailScrollOffset = sel;
    if (sel >= appState.detailScrollOffset + VISIBLE_DETAIL_ROWS)
        appState.detailScrollOffset = sel - VISIBLE_DETAIL_ROWS + 1;

    int maxOffset = max(0, numEvents - VISIBLE_DETAIL_ROWS);
    if (appState.detailScrollOffset > maxOffset)
        appState.detailScrollOffset = maxOffset;
}

static void draw_person_row(int personIdx, int rowY, bool isSelected) {
    Person& p = persons[personIdx];
    bool showDose = is_in_dose_window(personIdx)
        || appState.buzzerActive[personIdx]
        || appState.waitingForSensor[personIdx];

    uint16_t rowBg = (personIdx % 2 == 0) ? C_ROW_EVEN : C_ROW_ODD;
    if (isSelected) rowBg = C_SELECT_BG;
    tft.fillRect(0, rowY, SCREEN_W, ROW_H, rowBg);

    if (isSelected)
        tft.fillRect(0, rowY, 3, ROW_H, C_SELECT_ACC);

    tft.setTextSize(1);
    tft.setTextColor(isSelected ? C_SELECT_ACC : C_TEXT, rowBg);
    tft.setCursor(7, rowY + ROW_H / 2 - 4);
    tft.print(p.name);

    if (showDose && !appState.doseDelivered[personIdx]) {
        tft.fillRoundRect(SCREEN_W - 46, rowY + 8, 38, 14, 4, C_WARN_BG);
        tft.setTextColor(C_WARN_TXT, C_WARN_BG);
        tft.setCursor(SCREEN_W - 40, rowY + 12);
        tft.print("Dawka");
    }

    tft.drawFastHLine(0, rowY + ROW_H - 1, SCREEN_W, C_SEP);
}

static void draw_person_detail(int personIdx) {
    Person& p = persons[personIdx];
    int nowMin = rtc.getHour(true) * 60 + rtc.getMinute();

    tft.fillScreen(C_BG);
    draw_header();

    const int boxX = 4;
    const int boxY = HEADER_H + 4;
    const bool showDetailScroll = p.numEvents > VISIBLE_DETAIL_ROWS;
    const int boxW = showDetailScroll ? SCREEN_W - 14 : SCREEN_W - 8;

    tft.fillRoundRect(boxX, boxY, SCREEN_W - 8, 14, 4, C_HEADER_BG);
    tft.setTextColor(C_HEADER_TXT, C_HEADER_BG);
    tft.setTextSize(1);
    tft.setCursor(boxX + 6, boxY + 3);
    tft.print(p.name);

    tft.fillRect(boxX, DETAIL_LIST_Y, boxW + 6, DETAIL_LIST_BOTTOM - DETAIL_LIST_Y, C_BG);

    const int firstEvent = appState.detailScrollOffset;

    for (int i = 0; i < VISIBLE_DETAIL_ROWS; i++) {
        int e = firstEvent + i;
        int y = DETAIL_LIST_Y + i * DETAIL_ROW_PITCH;

        if (e >= p.numEvents) {
            tft.fillRect(boxX, y, boxW, DETAIL_ROW_H, C_BG);
            continue;
        }

        int hh, mm, ss;
        sscanf(p.events[e].time, "%d:%d:%d", &hh, &mm, &ss);
        int evMin = hh * 60 + mm;
        bool active = (nowMin >= evMin && nowMin < evMin + 5);
        bool isCursorHere = (appState.detailSelectedIndex == e);
        bool isChecked = p.events[e].checked;

        uint16_t rowBg = isCursorHere ? C_SELECT_BG : (active ? C_WARN_BG : ((e % 2 == 0) ? C_ROW_EVEN : C_ROW_ODD));
        uint16_t rowFg = isCursorHere ? C_SELECT_ACC : (active ? C_WARN_TXT : C_TEXT);

        tft.fillRoundRect(boxX, y, boxW, DETAIL_ROW_H, 3, rowBg);
        if (isCursorHere)
            tft.fillRect(boxX, y, 3, DETAIL_ROW_H, C_SELECT_ACC);
        else
            tft.drawRoundRect(boxX, y, boxW, DETAIL_ROW_H, 3, active ? C_WARN_TXT : C_SEP);

        tft.drawRect(boxX + 6, y + 3, 12, 12, rowFg);
        if (isChecked)
            tft.fillRect(boxX + 8, y + 5, 8, 8, rowFg);

        tft.setTextSize(1);
        tft.setTextColor(rowFg, rowBg);

        char timeBuf[6];
        snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", hh, mm);
        tft.setCursor(boxX + 22, y + 5);
        tft.print(timeBuf);

        char medDoseBuf[48];
        snprintf(medDoseBuf, sizeof(medDoseBuf), "%s x%d", p.events[e].name, p.events[e].dosage);
        tft.setCursor(boxX + 54, y + 5);
        tft.print(medDoseBuf);

        if (active && !isCursorHere) {
            tft.fillRoundRect(boxX + boxW - 40, y + 2, 36, 14, 4, C_WARN_TXT);
            tft.setTextColor(C_WARN_BG, C_WARN_TXT);
            tft.setCursor(boxX + boxW - 35, y + 6);
            tft.print("Dawka!");
        }
    }

    draw_detail_scrollbar(appState.detailScrollOffset, p.numEvents);

    auto draw_action_btn = [&](int btnX, int btnW, bool selected, const char* label) {
        uint16_t btnBg = selected ? C_SELECT_BG : C_ROW_EVEN;
        uint16_t btnFg = selected ? C_SELECT_ACC : C_TEXT;
        tft.fillRoundRect(btnX, DETAIL_ACTION_Y, btnW, 18, 3, btnBg);
        if (selected)
            tft.fillRect(btnX, DETAIL_ACTION_Y, 3, 18, C_SELECT_ACC);
        else
            tft.drawRoundRect(btnX, DETAIL_ACTION_Y, btnW, 18, 3, C_SEP);
        tft.setTextColor(btnFg, btnBg);
        tft.setCursor(btnX + (btnW / 2) - (strlen(label) * 3), DETAIL_ACTION_Y + 5);
        tft.print(label);
    };

    const int halfW = (SCREEN_W - 12) / 2;
    draw_action_btn(boxX, halfW, appState.detailSelectedIndex == p.numEvents, "Zatwierdz");
    draw_action_btn(boxX + halfW + 4, halfW, appState.detailSelectedIndex == p.numEvents + 1, "Cofnij");

    draw_footer();
}

void draw_footer() {
    tft.fillRect(0, FOOTER_Y, SCREEN_W, FOOTER_H, C_FOOTER_BG);
    tft.drawFastHLine(0, FOOTER_Y, SCREEN_W, C_SEP);
    tft.setTextColor(C_FOOTER_TXT, C_FOOTER_BG);
    tft.setTextSize(1);
    tft.setCursor(4, FOOTER_Y + 2);
    tft.print(appState.detailView ? "UP/DN-kursor OK-akcja" : "UP/DN-wybor  OK-pokaz leki");
}

void draw_ui() {
    power_ensure_display_on();

    if (appState.detailView) {
        draw_person_detail(appState.selectedIndex);
        return;
    }

    draw_header();
    for (int i = 0; i < VISIBLE_ROWS; i++) {
        int personIdx = appState.scrollOffset + i;
        if (personIdx >= NUM_PERSONS) {
            tft.fillRect(0, LIST_Y + i * ROW_H, SCREEN_W - 4, ROW_H, C_BG);
            continue;
        }
        draw_person_row(personIdx, LIST_Y + i * ROW_H, personIdx == appState.selectedIndex);
    }
    draw_scrollbar(appState.scrollOffset);
    draw_footer();
}
