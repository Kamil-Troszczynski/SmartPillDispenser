#include "sensors.hpp"
#include "app_state.hpp"
#include "persons.hpp"
#include "screen.hpp"
#include "dfns_consts_libs.hpp"

static int lastSensorState = HIGH;

void setup_sensors() {
    pinMode(SENSOR_PIN, INPUT_PULLUP);
    lastSensorState = digitalRead(SENSOR_PIN);
    Serial.printf("Fotokomorka gotowa (GPIO %d)\n", SENSOR_PIN);
}

void handle_sensors() {
    int state = digitalRead(SENSOR_PIN);

    if (!state && lastSensorState) {
        Serial.println("Fotokomorka: przerwanie wiazki");

        for (int i = 0; i < NUM_PERSONS; i++) {
            if (appState.waitingForSensor[i]) {
                appState.waitingForSensor[i] = false;
                appState.doseDelivered[i] = true;
                Serial.printf("Dawka potwierdzona: %s\n", persons[i].name);
                draw_ui();
                break;
            }
        }
    }

    if (state && !lastSensorState) {
        Serial.println("Fotokomorka: wiazka wrocila");
    }

    lastSensorState = state;
}
