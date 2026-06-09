#include "sensors.hpp"
#include "app_state.hpp"
#include "persons.hpp"
#include "screen.hpp"
#include "servos.hpp"
#include "dfns_consts_libs.hpp"

static int lastSensorState[NUM_DISPENSERS];

void setup_sensors() {
    for (int i = 0; i < NUM_DISPENSERS; i++) {
        pinMode(SENSOR_PINS[i], INPUT_PULLUP);
        lastSensorState[i] = digitalRead(SENSOR_PINS[i]);
    }
}

void handle_sensors() {
    for (int slot = 0; slot < NUM_DISPENSERS; slot++) {
        int state = digitalRead(SENSOR_PINS[slot]);

        // Wykryto opadające zbocze (przerwanie wiązki IR / wciśnięcie krańcówki)
        if (!state && lastSensorState[slot]) {
            for (int i = 0; i < NUM_PERSONS; i++) {
                if (appState.waitingForSensor[i]) {
                    
                    // Sprawdzamy, czy aktywowany slot należy do jakiegokolwiek leku tej osoby
                    bool slotMatch = false;
                    for (int e = 0; e < persons[i].numEvents; e++) {
                        if (event_dispenser_slot(i, e) == slot) {
                            slotMatch = true;
                            break; // Znaleziono powiązanie, przerywamy pętlę wewnętrzną
                        }
                    }

                    // Jeśli czujnik zadziałał na prawidłowym slocie
                    if (slotMatch) {
                        appState.waitingForSensor[i] = false;
                        appState.doseDelivered[i] = true;
                        Serial.printf("Dawka potwierdzona: %s (slot %d)\n", persons[i].name, slot);
                        draw_ui();
                        break;
                    }
                }
            }
        }

        lastSensorState[slot] = state;
    }
}