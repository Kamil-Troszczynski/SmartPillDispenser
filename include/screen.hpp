#pragma once

#include "dfns_consts_libs.hpp"

void draw_header();
void draw_footer();
void draw_ui();
void draw_sync_status(const char* line1, const char* line2, uint16_t bg, uint16_t fg);
void sync_detail_scroll(int numEvents);
