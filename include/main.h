//
// Created by chriss on 02.08.22.
//
#pragma once

//#include <IRremote.hpp>
#include <Adafruit_NeoPixel.h>


#ifndef UNTITLED1_MAIN_H
#define UNTITLED1_MAIN_H

#define UPPER_LIMIT_SENSOR_PIN 34
#define LOWER_LIMIT_SENSER_PIN 35
#define UP_BUTTON_PIN 25
#define STOP_BUTTON_PIN 26
#define DOWN_BUTTON_PIN 27
#define IR_SEND_PIN     5
#define ALED_PIN    13

#define NUM_LED     11
void checkLimitSwitches();
void checkButtons();


enum state {
    up,
    down,
    stopped
};

int lastLedUpdate;

state state = stopped;
u_int8_t swipeLen = 5;
u_int16_t swipePos = 0;
u_int16_t swipeCycle = 0;


#endif //UNTITLED1_MAIN_H
