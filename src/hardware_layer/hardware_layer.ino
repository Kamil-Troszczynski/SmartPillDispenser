#include <WiFi.h>
#include <ESP32Servo.h>
#include <ESP32Time.h>
#include "time.h"
#include "access.ino"

const char* ssid             = WIFI_SSID;
const char* password         = WIFI_PASSWORD;

const char* ntpServer        = "pool.ntp.org";
const long  gmtOffset_sec    = 0;
const int   daylightOffset_sec = 3600;   // summer time PL = +1h (winter time = 0)

// hour and minute when servo should be moved
const int TARGET_HOUR   = 1;
const int TARGET_MINUTE = 26;

// Servo Pin
static const int servoPin = 13;

// Servo Positions
const int SERVO_START = 0;
const int SERVO_END   = 90;

Servo    myservo;
ESP32Time rtc(3600);

bool servoMoved    = false;
int  lastCheckedDay = -1;


void syncTimeFromNTP() {
  Serial.print("Connecting with WiFi...");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected. Requested time from NTP...");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;
    int retries = 0;
    while (!getLocalTime(&timeinfo) && retries < 10) {
      delay(500);
      retries++;
    }

    if (getLocalTime(&timeinfo)) {
      rtc.setTimeStruct(timeinfo);
      Serial.println("Synchronised time with NTP");
      Serial.println(&timeinfo, "Date/time: %A, %d %B %Y  %H:%M:%S");
    } else {
      Serial.println("Time from NTP not requested");
    }
  } else {
    Serial.println("\nNo WiFi connection, time not synchronised");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("Turned off WiFi");
}

void checkAndMoveServo() {
  int currentHour   = rtc.getHour(true); // 24h form
  int currentMinute = rtc.getMinute();
  int currentDay    = rtc.getDay();

  if (currentDay != lastCheckedDay) {
    servoMoved    = false;
    lastCheckedDay = currentDay;
    Serial.println("New day – servo's flag reset");
  }

  if (!servoMoved &&
      currentHour   == TARGET_HOUR &&
      currentMinute == TARGET_MINUTE)
  {
    Serial.println(">>> Target time, servo is moving...");
    myservo.write(SERVO_END);
    delay(1000);              
    myservo.write(SERVO_START);
    servoMoved = true;        
    Serial.println(">>> Servo move back to start position");
  }
}

void printCurrentTime() {
  Serial.printf("Time RTC: %02d:%02d:%02d  |  day: %d\n",
                rtc.getHour(true),
                rtc.getMinute(),
                rtc.getSecond(),
                rtc.getDay());
}

void setup() {
  Serial.begin(115200);
  myservo.attach(servoPin);
  myservo.write(SERVO_START);

  syncTimeFromNTP();
}

void loop() {
  checkAndMoveServo();
  printCurrentTime();
  delay(1000);
}