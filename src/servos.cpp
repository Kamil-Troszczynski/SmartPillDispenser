#include "servos.hpp"


void setup_up_servos(Adafruit_PWMServoDriver & pca){
  pca.begin();
  pca.setPWMFreq(SERVO_FREQ);

  for (int i = 0; i < NUM_SERVOS; i++) {
    pca.setPWM(SERVO_CHANNELS[i], 0, SERVO_MIN_PULSE);
  }
}

void move_servo(int servoIndex, Adafruit_PWMServoDriver& pca) {
  for (int pos = SERVO_MIN_PULSE; pos <= SERVO_MAX_PULSE; pos += 1) {
    pca.setPWM(SERVO_CHANNELS[servoIndex], 0, pos);
    delay(15);
  }
 
  delay(2000);
 
  for (int pos = SERVO_MAX_PULSE; pos >= SERVO_MIN_PULSE; pos -= 1) {
    pca.setPWM(SERVO_CHANNELS[servoIndex], 0, pos);
    delay(15);
  }
 
  delay(2000);
}