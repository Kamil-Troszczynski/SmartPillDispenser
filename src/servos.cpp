#include "servos.hpp"
#include "persons.hpp"

static void move_servo_channel(uint8_t channel, Adafruit_PWMServoDriver& pca) {
    pca.setPWM(channel, 0, SERVO_MAX_PULSE);
    delay(800);
    pca.setPWM(channel, 0, SERVO_MIN_PULSE);
    delay(800);
}

void setup_up_servos(Adafruit_PWMServoDriver& pca) {
    pca.begin();
    pca.setPWMFreq(SERVO_FREQ);
    for (int i = 0; i < NUM_DISPENSERS; i++)
        pca.setPWM(SERVO_CHANNELS[i], 0, SERVO_MIN_PULSE);
}

int event_dispenser_slot(int personIdx, int eventIdx) {
    if (personIdx < 0 || personIdx >= NUM_PERSONS) return 0;
    if (eventIdx < 0 || eventIdx >= persons[personIdx].numEvents) return 0;
    int chamber = persons[personIdx].events[eventIdx].chamberNumber;
    int slot = chamber - 1;

    if (slot < 0 || slot >= NUM_DISPENSERS) {
        Serial.print("BLAD: Komora poza zakresem! Wymuszam 0.");
        slot = 0; 
    }
    return slot;
}

void dispense_servo(Adafruit_PWMServoDriver& pca, int personIdx, int eventIdx) {
    int slot = event_dispenser_slot(personIdx, eventIdx);
    
    Serial.print("Wyrzut tabletki: "); 
    Serial.print(persons[personIdx].name);
    Serial.print(" (slot "); Serial.print(slot); Serial.println(")");
    
    move_servo_channel(SERVO_CHANNELS[slot], pca);
}
