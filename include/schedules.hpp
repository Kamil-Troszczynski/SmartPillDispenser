#pragma once

#include <cstdint>
#include "dfns_consts_libs.hpp"
#include "persons.hpp"
#include "app_state.hpp"

bool check_schedules();
bool is_in_window(int personIdx);
uint64_t us_until_next_schedule_wake();
