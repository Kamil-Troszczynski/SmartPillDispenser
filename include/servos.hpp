#pragma once

#include "dfns_consts_libs.hpp"

void setup_up_servos(Adafruit_PWMServoDriver& pca);
void dispense_servo(Adafruit_PWMServoDriver& pca, int personIdx, int eventIdx);
int event_dispenser_slot(int personIdx, int eventIdx);