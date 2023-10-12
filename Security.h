#ifndef _SECUTITY_H_
#define _SECUTITY_H_

#include "mqtt.h"

const int sensorPin = 15;
const int buzzerPin = 12;


void Alert() {
    for (int i = 0; i < 180; i++) {
        float sinVal = (sin(i * (3.1412 / 180)));
        int toneVal = 2000 + (int(sinVal * 1000));
        tone(buzzerPin, toneVal);
        delay(2);
    }
}

int MotionDetection() {
    if (AllowMovement == false) {
      // noTone(buzzerPin);
      return -1;
    }
        
    int State = digitalRead(sensorPin);
    ei_printf("%d", State);

    if (State == HIGH) {
        // Alert();
        ei_printf("Motion detected\n");
        char* Event = "warningON";
        sendRequest(appHost, 0, Event);
        return 1;
    }
    // noTone(buzzerPin);

    ei_printf("No motion detected\n");
    return 0;
}

void setupSecurity() {
    pinMode(sensorPin, INPUT);
    pinMode(buzzerPin, OUTPUT);
}


#endif