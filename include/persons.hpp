#pragma once

#include "dfns_consts_libs.hpp"


struct Person {
  const char* name;
  int buzzerStartHour;
  int buzzerStartMin;
  int buzzerEndHour;
  int buzzerEndMin;
  int servoIndex;
};

extern Person persons[NUM_PERSONS];