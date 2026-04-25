#include "mqtt_broker.hpp"


const char* MQTT_BROKER   = "192.168.1.100";
const int   MQTT_PORT     = 1883;
const char* MQTT_CLIENT   = "esp32_client_1";
const char* MQTT_USER     = "";
const char* MQTT_PASSWORD = "";   


const char* TOPIC_PUB = "pub_confirmation";



const char* TOPIC_SUB = "sub_change_schedule";


WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);


void on_message(char* topic, byte* payload, unsigned int length){
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.printf("Wiadomość [%s]: %s\n", topic, message.c_str());

    if (String(topic) == "pub_confirmation") {
        // Something will be here
    }
};


void connect_mqtt(){
    while (!mqtt.connected()) {
        Serial.print("Laczenie MQTT...");

        bool connected = (strlen(MQTT_USER) > 0)
            ? mqtt.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASSWORD)
            : mqtt.connect(MQTT_CLIENT);

        if (connected) {
            Serial.println("OK");
            mqtt.subscribe(TOPIC_SUB);
            mqtt.publish(TOPIC_PUB, "online");
        } else {
            Serial.printf("Blad: %d, ponów za 3s\n", mqtt.state());
            delay(3000);
        }
    }
};