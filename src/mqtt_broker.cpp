#include "mqtt_broker.hpp"
#include "persons.hpp"
#include "screen.hpp"
#include "app_state.hpp"
#include "power.hpp"
#include <WiFi.h>

static WiFiClient   espClient;
static PubSubClient mqttClient(espClient);

static const char* _topic_sync;
static const char* _topic_confirmation;

bool syncReceived = false;
static void processSync(const char* message, unsigned int length) {
    JsonDocument doc;
    if (deserializeJson(doc, message, length)) {
        Serial.println("MQTT: blad parsowania JSON");
        return;
    }

    NUM_PERSONS = 0;
    memset(persons, 0, sizeof(persons));
    memset(appState.buzzerActive,     false, sizeof(appState.buzzerActive));
    memset(appState.buzzerAcked,      false, sizeof(appState.buzzerAcked));
    memset(appState.doseDelivered,    false, sizeof(appState.doseDelivered));
    memset(appState.waitingForSensor, false, sizeof(appState.waitingForSensor));

    for (JsonObject user : doc["users"].as<JsonArray>()) {
        if (NUM_PERSONS >= MAX_PERSONS) break;

        Person& p = persons[NUM_PERSONS];
        strncpy(p.name, user["username"] | "?", sizeof(p.name) - 1);
        p.numEvents = 0;

        for (JsonObject med : user["medications"].as<JsonArray>()) {
            int current_chamber = med["chamber_number"] | 1;

            for (JsonObject ev : med["dispense_events"].as<JsonArray>()) {
                if (p.numEvents >= MAX_EVENTS_PER_PERSON) break;

                DispenseEvent& de = p.events[p.numEvents];
                strncpy(de.name, med["med_name"] | "Nieznany lek", sizeof(de.name) - 1);
                strncpy(de.time, ev["time"] | "00:00:00", sizeof(de.time) - 1);
                de.dosage = ev["dosage"] | 1;
                de.dts_id = ev["dts_id"] | 0;
                de.chamberNumber = current_chamber; 
                
                p.numEvents++;
            }
        }

        Serial.printf("MQTT sync: %s events=%d\n", p.name, p.numEvents);
        NUM_PERSONS++;
    }

    syncReceived = true;
    power_notify_user_activity();
    draw_ui();
}

static void onMessage(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, _topic_sync) == 0)
        processSync((const char*)payload, length);
}

static void connectMqtt() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect("esp32-client-1")) {
            mqttClient.subscribe(_topic_sync, 1);
        } else {
            delay(5000);
        }
    }
}

void mqtt_init(const char* broker_host, int broker_port,
               const char* topic_sync, const char* topic_confirmation) {
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
    ev["dts_id"] = dts_id;
    ev["dosage"] = dosage;

    char buf[256];
    serializeJson(doc, buf);
    mqttClient.publish(_topic_confirmation, buf);
}
