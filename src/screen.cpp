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

    if (line2 != nullptr && line2[0] != '\0') {
        tft.setCursor(boxX + 8, boxY + 21);
        tft.print(line2);
    }
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


// czy osoba ma aktywną dawkę do podania
bool person_needs_dose(int personIdx) {
    Person& p = persons[personIdx];
    int nowMin = rtc.getHour(true) * 60 + rtc.getMinute();
    for (int e = 0; e < p.numEvents; e++) {
        int hh, mm, ss;
        sscanf(p.events[e].time, "%d:%d:%d", &hh, &mm, &ss);
        int evMin = hh * 60 + mm;
        if (nowMin >= evMin && nowMin < evMin + 5) return true;
    }
    return false;
}


void draw_person_row(int personIdx, int rowY, bool isSelected) {
    Person& p = persons[personIdx];
    bool needsDose = person_needs_dose(personIdx);
    bool buzzing   = appState.buzzerActive[personIdx];
    bool showDose  = needsDose || buzzing || appState.waitingForSensor[personIdx];

    uint16_t rowBg = (personIdx % 2 == 0) ? C_ROW_EVEN : C_ROW_ODD;
    if (isSelected) rowBg = C_SELECT_BG;
    tft.fillRect(0, rowY, SCREEN_W, ROW_H, rowBg);

    if (isSelected) {
        tft.fillRect(0, rowY, 3, ROW_H, C_SELECT_ACC);
    }

    tft.setTextSize(1);
    tft.setTextColor(isSelected ? C_SELECT_ACC : C_TEXT, rowBg);
    tft.setCursor(7, rowY + ROW_H / 2 - 4);  // wyśrodkowany pionowo — samo imię
    tft.print(p.name);

    // badge "Dawka" przy imieniu jeśli aktywna
    if (showDose && !appState.doseDelivered[personIdx]) {
        tft.fillRoundRect(SCREEN_W - 46, rowY + 8, 38, 14, 4, C_WARN_BG);
        tft.setTextColor(C_WARN_TXT, C_WARN_BG);
        tft.setCursor(SCREEN_W - 40, rowY + 12);
        tft.print("Dawka");
    }

    tft.drawFastHLine(0, rowY + ROW_H - 1, SCREEN_W, C_SEP);
}

void draw_person_detail(int personIdx) {
    Person& p = persons[personIdx];
    int nowMin = rtc.getHour(true) * 60 + rtc.getMinute();

    // tło ramki
    tft.fillScreen(C_BG);
    draw_header();

    // nagłówek ramki z imieniem
    const int boxX = 4;
    const int boxY = HEADER_H + 4;
    const int boxW = SCREEN_W - 8;

    tft.fillRoundRect(boxX, boxY, boxW, 16, 4, C_HEADER_BG);
    tft.setTextColor(C_HEADER_TXT, C_HEADER_BG);
    tft.setTextSize(1);
    tft.setCursor(boxX + 6, boxY + 4);
    tft.print(p.name);

    int y = boxY + 22;

    // --- 1. LISTA ZDARZEŃ (LEKÓW) ---
    for (int e = 0; e < p.numEvents && y < FOOTER_Y - 24; e++) {
        int hh, mm, ss;
        sscanf(p.events[e].time, "%d:%d:%d", &hh, &mm, &ss);
        int evMin = hh * 60 + mm;
        bool active = (nowMin >= evMin && nowMin < evMin + 5);

        // Sprawdzanie stanu zaznaczenia i kursora
        bool isCursorHere = (appState.detailSelectedIndex == e);
        bool isChecked = p.events[e].checked;

        // Kolory dynamiczne zależne od kursora
        uint16_t rowBg = isCursorHere ? C_SELECT_BG : (active ? C_WARN_BG : ((e % 2 == 0) ? C_ROW_EVEN : C_ROW_ODD));
        uint16_t rowFg = isCursorHere ? C_SELECT_ACC : (active ? C_WARN_TXT : C_TEXT);

        // Tło rzędu + lewy akcent jeśli najeżdżamy kursorem
        tft.fillRoundRect(boxX, y, boxW, 20, 3, rowBg);
        if (isCursorHere) {
            tft.fillRect(boxX, y, 3, 20, C_SELECT_ACC); 
        } else {
            tft.drawRoundRect(boxX, y, boxW, 20, 3, active ? C_WARN_TXT : C_SEP);
        }

        // Rysowanie Checkboxa [ ] lub [x]
        tft.drawRect(boxX + 8, y + 4, 12, 12, rowFg);
        if (isChecked) {
            tft.fillRect(boxX + 10, y + 6, 8, 8, rowFg); // Wypełniony środek jeśli "checked"
        }

        tft.setTextSize(1);
        tft.setTextColor(rowFg, rowBg);

        // Czas
        char timeBuf[6];
        snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", hh, mm);
        tft.setCursor(boxX + 26, y + 6);
        tft.print(timeBuf);

        // Nazwa leku i dawka
        char medDoseBuf[64];
        snprintf(medDoseBuf, sizeof(medDoseBuf), "%s - x%d", p.events[e].name, p.events[e].dosage);
        tft.setCursor(boxX + 62, y + 6);
        tft.print(medDoseBuf);

        // Badge Dawka (jeśli rzutuje i nie zasłania go podświetlenie)
        if (active && !isCursorHere) {
            tft.fillRoundRect(SCREEN_W - 50, y + 3, 42, 14, 4, C_WARN_TXT);
            tft.setTextColor(C_WARN_BG, C_WARN_TXT);
            tft.setCursor(SCREEN_W - 44, y + 7);
            tft.print("Dawka!");
        }

        y += 24;
    }

    // --- 2. PRZYCISK "ZATWIERDŹ" NA DOLE ---
    // Kursor traktuje ten guzik jako pozycję `numEvents` (jeden indeks za ostatnim lekiem)
    bool isConfirmSelected = (appState.detailSelectedIndex == p.numEvents);
    uint16_t btnBg = isConfirmSelected ? C_SELECT_BG : C_ROW_EVEN;
    uint16_t btnFg = isConfirmSelected ? C_SELECT_ACC : C_TEXT;

    tft.fillRoundRect(boxX, y, boxW, 20, 3, btnBg);
    if (isConfirmSelected) {
        tft.fillRect(boxX, y, 3, 20, C_SELECT_ACC);
    } else {
        tft.drawRoundRect(boxX, y, boxW, 20, 3, C_SEP);
    }
    
    tft.setTextColor(btnFg, btnBg);
    tft.setCursor(boxX + (boxW / 2) - 45, y + 6); // Zgrubne środkowanie
    tft.print("Zatwierdz wybrane");

    // stopka
    draw_footer();
}

void draw_footer() {
    tft.fillRect(0, FOOTER_Y, SCREEN_W, FOOTER_H, C_FOOTER_BG);
    tft.drawFastHLine(0, FOOTER_Y, SCREEN_W, C_SEP);
    tft.setTextColor(C_FOOTER_TXT, C_FOOTER_BG);
    tft.setTextSize(1);
    tft.setCursor(4, FOOTER_Y + 2);
    
    // Dynamiczny tekst w zależności od widoku
    if (appState.detailView) {
        tft.print("UP/DN-kursor  OK-zaznacz/zatw");
    } else {
        tft.print("UP/DN-wybor  OK-pokaz leki");
    }
}

void draw_ui() {
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