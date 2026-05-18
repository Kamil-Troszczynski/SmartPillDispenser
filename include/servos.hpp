#pragma once

#include "dfns_consts_libs.hpp"


void setup_up_servos(Adafruit_PWMServoDriver & pca);


void move_servo(int servoIndex, Adafruit_PWMServoDriver& pca);