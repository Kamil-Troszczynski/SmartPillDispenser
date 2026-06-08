#pragma once 

#include "dfns_consts_libs.hpp"

void draw_header();
void draw_scrollbar(int scrollOffset);
void draw_person_row(int personIdx, int rowY, bool isSelected);
void draw_footer();
void draw_ui();
void sync_detail_scroll(int numEvents);
void draw_sync_status(const char* line1, const char* line2, uint16_t bg, uint16_t fg);