#include "mqtt_broker.hpp"
#include "persons.hpp"
#include "screen.hpp"
#include "app_state.hpp"
#include "power.hpp"
#include <WiFi.h>

static WiFiClient    espClient;
static PubSubClient  mqttClient(espClient);

static const char*   _topic_sync;
static const char*   _topic_confirmation;

bool syncReceived = false;

// --- parsowanie payloadu sync ---
static void processSync(const char* message, unsigned int length) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, message, length);
    if (err) {
        Serial.println("MQTT: błąd parsowania sync JSON");
        return;
    }

    NUM_PERSONS = 0;
    memset(persons, 0, sizeof(persons));
    memset(appState.buzzerActive,      false, sizeof(appState.buzzerActive));
    memset(appState.buzzerAcked,       false, sizeof(appState.buzzerAcked));
    memset(appState.doseDelivered,     false, sizeof(appState.doseDelivered));
    memset(appState.waitingForSensor,  false, sizeof(appState.waitingForSensor));

    for (JsonObject user : doc["users"].as<JsonArray>()) {
        if (NUM_PERSONS >= MAX_PERSONS) break;

        Person& p = persons[NUM_PERSONS];

        strncpy(p.name, user["username"] | "?", sizeof(p.name) - 1);
        p.numEvents    = 0;
        p.servoIndex   = NUM_PERSONS + 1; // domyślnie servo = kolejny numer
        p.chamberNumber = -1;

        // zbierz zdarzenia ze wszystkich leków tego użytkownika
        for (JsonObject med : user["medications"].as<JsonArray>()) {
            p.chamberNumber = med["chamber_number"] | -1;
            p.servoIndex    = p.chamberNumber;

            for (JsonObject ev : med["dispense_events"].as<JsonArray>()) {
                if (p.numEvents >= MAX_EVENTS_PER_PERSON) break;

                DispenseEvent& de = p.events[p.numEvents];
                strncpy(de.name, med["med_name"] | "Nieznany lek", sizeof(de.name) - 1);
                strncpy(de.time, ev["time"] | "00:00:00", sizeof(de.time) - 1);
                de.dosage = ev["dosage"] | 1;
                de.dts_id = ev["dts_id"] | 0;
                p.numEvents++;
            }
        }

        // ustaw okno buzzer na podstawie pierwszego zdarzenia
        if (p.numEvents > 0) {
            int hh = 0, mm = 0, ss = 0;
            sscanf(p.events[0].time, "%d:%d:%d", &hh, &mm, &ss);
            p.buzzerStartHour = hh;
            p.buzzerStartMin  = mm;
            // okno 5 minut od czasu zdarzenia
            int endTotal = hh * 60 + mm + 5;
            p.buzzerEndHour = endTotal / 60;
            p.buzzerEndMin  = endTotal % 60;
        }

        Serial.printf("MQTT sync: user=%s chamber=%d events=%d\n",
                      p.name, p.chamberNumber, p.numEvents);
        NUM_PERSONS++;
    }

    syncReceived = true;
    Serial.printf("MQTT sync: załadowano %d użytkowników\n", NUM_PERSONS);
    power_notify_user_activity();
    draw_ui();
}

// --- callback ---
static void onMessage(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, _topic_sync) == 0) {
        processSync((const char*)payload, length);
    }
}

// --- połączenie ---
static void connectMqtt() {
    while (!mqttClient.connected()) {
        Serial.print("MQTT: łączenie...");
        if (mqttClient.connect("esp32-client-1")) {
            Serial.println("OK");
            mqttClient.subscribe(_topic_sync, 1);
        } else {
            Serial.printf("błąd rc=%d, retry 5s\n", mqttClient.state());
            delay(5000);
        }
    }
}

// --- API publiczne ---
void mqtt_init(const char* broker_host, int broker_port, 
               const char* topic_sync,
               const char* topic_confirmation) {
    _topic_sync         = topic_sync;
    _topic_confirmation = topic_confirmation;

    mqttClient.setServer(broker_host, broker_port);
    mqttClient.setBufferSize(8192);
    mqttClient.setCallback(onMessage);
    connectMqtt();
}

void mqtt_loop() {
    if (!mqttClient.connected()) connectMqtt();
    mqttClient.loop();
}

void mqtt_publish_confirmation(int dts_id, int dosage) {
    JsonDocument doc;
    JsonArray medications = doc["medications"].to<JsonArray>();
    JsonObject med        = medications.add<JsonObject>();
    JsonArray events      = med["dispense_events"].to<JsonArray>();
    JsonObject ev         = events.add<JsonObject>();
    ev["dts_id"]  = dts_id;
    ev["dosage"]  = dosage;

    char buf[256];
    serializeJson(doc, buf);
    mqttClient.publish(_topic_confirmation, buf);
    Serial.printf("MQTT: potwierdzenie dts_id=%d\n", dts_id);
}