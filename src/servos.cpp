#include "servos.hpp"
#include "dfns_consts_libs.hpp"


void move_servo(int servoIndex, Servo (&servos)[NUM_SERVOS]) {
  if (servoIndex < 0 || servoIndex >= NUM_SERVOS) return;
  servos[servoIndex].write(SERVO_END);
  delay(1000);
  servos[servoIndex].write(SERVO_START);
}