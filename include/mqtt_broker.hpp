#pragma once
#include <PubSubClient.h>
#include <ArduinoJson.h>

void mqtt_init(const char* broker_host, int broker_port,  const char* topic_sync, const char* topic_confirmation);
void mqtt_loop();
void mqtt_publish_confirmation(int dts_id, int dosage);

extern bool syncReceived;