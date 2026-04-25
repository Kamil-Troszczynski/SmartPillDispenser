#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


void on_message(char* topic, byte* payload, unsigned int length);
void connect_mqtt();