#pragma once
#include <Arduino.h>

#define MAX_PERSONS 16
#define MAX_EVENTS_PER_PERSON 8

struct DispenseEvent {
    char name[32];
    char time[10];
    int dosage;
    int dts_id;
    bool checked = false;
};

struct Person {
    char name[32];
    int  servoIndex;
    int  chamberNumber;

    // okno harmonogramu (pierwsze zdarzenie dnia)
    int  buzzerStartHour;
    int  buzzerStartMin;
    int  buzzerEndHour;
    int  buzzerEndMin;

    DispenseEvent events[MAX_EVENTS_PER_PERSON];
    int           numEvents;
};

extern Person persons[MAX_PERSONS];
extern int    NUM_PERSONS;