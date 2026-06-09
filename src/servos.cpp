#include "servos.hpp"

static void move_servo_channel(uint8_t channel, Adafruit_PWMServoDriver& pca) {
    for (int pos = SERVO_MIN_PULSE; pos <= SERVO_MAX_PULSE; pos++) {
        pca.setPWM(channel, 0, pos);
        delay(15);
    }

    delay(2000);

    for (int pos = SERVO_MAX_PULSE; pos >= SERVO_MIN_PULSE; pos--) {
        pca.setPWM(channel, 0, pos);
        delay(15);
    }

    delay(2000);
}

void setup_up_servos(Adafruit_PWMServoDriver& pca) {
    pca.begin();
    pca.setPWMFreq(SERVO_FREQ);
    pca.setPWM(SERVO_DISPENSE_CHANNEL, 0, SERVO_MIN_PULSE);
}

void dispense_servo(Adafruit_PWMServoDriver& pca) {
    move_servo_channel(SERVO_DISPENSE_CHANNEL, pca);
}
