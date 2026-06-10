#pragma once

#include <Arduino.h>

#define MAX_PERSONS 16
#define MAX_EVENTS_PER_PERSON 8

struct DispenseEvent {
    char name[32];
    char time[16];
    int dosage;
    int dts_id;
    int chamberNumber; 
    bool checked;
};

struct Person {
    char name[32];
    int numEvents;
    DispenseEvent events[MAX_EVENTS_PER_PERSON];
};

extern Person persons[MAX_PERSONS];
extern int    NUM_PERSONS;