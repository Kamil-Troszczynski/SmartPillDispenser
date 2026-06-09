#include "servos.hpp"

static void move_servo_channel(uint8_t channel, Adafruit_PWMServoDriver& pca) {
    for (int pos = SERVO_MIN_PULSE; pos <= SERVO_MAX_PULSE; pos += 2) {
        pca.setPWM(channel, 0, pos);
        delay(5);
    }
    pca.setPWM(channel, 0, SERVO_MAX_PULSE);
    delay(300);

    for (int pos = SERVO_MAX_PULSE; pos >= SERVO_MIN_PULSE; pos -= 2) {
        pca.setPWM(channel, 0, pos);
        delay(5);
    }
    pca.setPWM(channel, 0, SERVO_MIN_PULSE);
    delay(300);
}

static void home_servo_channel(uint8_t channel, Adafruit_PWMServoDriver& pca) {
    for (int pos = SERVO_MAX_PULSE; pos >= SERVO_MIN_PULSE; pos -= 2) {
        pca.setPWM(channel, 0, pos);
        delay(5);
    }
    pca.setPWM(channel, 0, SERVO_MIN_PULSE);
    delay(300);
}

void setup_up_servos(Adafruit_PWMServoDriver& pca) {
    pca.begin();
    pca.setPWMFreq(SERVO_FREQ);
    home_servo_channel(SERVO_DISPENSE_CHANNEL, pca);
}

void dispense_servo(Adafruit_PWMServoDriver& pca) {
    move_servo_channel(SERVO_DISPENSE_CHANNEL, pca);
}

void home_servo(Adafruit_PWMServoDriver& pca) {
    home_servo_channel(SERVO_DISPENSE_CHANNEL, pca);
}