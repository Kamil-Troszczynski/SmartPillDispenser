#include <WiFi.h>
#include <ESP32Servo.h>
#include <ESP32Time.h>
#include "time.h"
#include "access.ino"


const char* ssid              = WIFI_SSID;
const char* password          = WIFI_PASSWORD;
const char* ntpServer         = "pool.ntp.org";
const long  gmtOffset_sec     = 0;
const int   daylightOffset_sec = 3600;


const int NUM_SERVOS = 4;

const int SERVO_PINS[NUM_SERVOS] = { 13, 12, 14, 27 };
const int BUTTON_PIN = 33;

const int SERVO_START = 0;
const int SERVO_END   = 180;


struct Schedule {
  int startHour;
  int startMin;
  int endHour;
  int endMin;
};


const Schedule SCHEDULES[NUM_SERVOS] = {
  {  13,  28, 13,  30 },   // Servo 0 (GPIO 13) - active 13:28-13:30
  {  13,  35, 13,  37 },   // Servo 1 (GPIO 12) - active 13:35-13:37
  {  13,  38, 13,  40 },   // Servo 2 (GPIO 14) - active 13:38-13:40
  {  13,  41, 13,  43 },   // Servo 3 (GPIO 27) - active 13:41-13:43
};


Servo     servos[NUM_SERVOS];
ESP32Time rtc(3600);

unsigned long lastPressTime = 0;
const unsigned long DEBOUNCE_MS = 300;


int to_minutes(int hour, int min) {
  return hour * 60 + min;
}

bool is_in_schedule(int servoIndex) {
  int now   = to_minutes(rtc.getHour(true), rtc.getMinute());
  int start = to_minutes(SCHEDULES[servoIndex].startHour, SCHEDULES[servoIndex].startMin);
  int end   = to_minutes(SCHEDULES[servoIndex].endHour,   SCHEDULES[servoIndex].endMin);
  return (now >= start && now < end);
}

void move_servo(int servoIndex) {
  Serial.printf(">>> Servo %d: rotation to %d°\n", servoIndex, SERVO_END);
  servos[servoIndex].write(SERVO_END);
  delay(1000);
  servos[servoIndex].write(SERVO_START);
  Serial.printf(">>> Servo %d: home...\n", servoIndex);
}

void check_button() {
  if (digitalRead(BUTTON_PIN) != LOW) return;
  
  unsigned long now = millis();
  if (now - lastPressTime <= DEBOUNCE_MS) return;
  lastPressTime = now;

  // Find first servo in window schedule
  bool anyActive = false;
  for (int i = 0; i < NUM_SERVOS; i++) {
    if (is_in_schedule(i)) {
      anyActive = true;
      move_servo(i);
      break;  // launch only first appropriate
    }
  }

  if (!anyActive) {
    Serial.println("Button clicked. No servo is in schedule");
  }
}

void print_current_time() {
  Serial.printf("RTC: %02d:%02d:%02d\n",
                rtc.getHour(true), rtc.getMinute(), rtc.getSecond());
}


void sync_time_from_NTP() {
  Serial.print("Connecting WiFi... ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected WiFi. Request time from NTP...");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    int retries = 0;
    while (!getLocalTime(&timeinfo) && retries < 10) {
      delay(500);
      retries++;
    }
    if (getLocalTime(&timeinfo)) {
      rtc.setTimeStruct(timeinfo);
      Serial.println("Synchronised time");
      Serial.println(&timeinfo, "Date/Time: %A, %d %B %Y  %H:%M:%S");
    } else {
      Serial.println("Request time from NTP failed");
    }
  } else {
    Serial.println("\nNo WiFi - time was not synchronised!");
  }
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("Turned off WiFi");
}


void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(SERVO_START);
  }
  sync_time_from_NTP();
}


void loop() {
  check_button();
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 10000) {
    lastPrint = millis();
    print_current_time();
  }
}