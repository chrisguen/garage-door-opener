#include <Arduino.h>
#include "main.h"

Adafruit_NeoPixel pixels(NUM_LED, ALED_PIN, NEO_GRBW + NEO_KHZ800);

void updateLeds();

void setup() {
    delay(200);
    pixels.setBrightness(100);
    lastLedUpdate = millis();
    Serial.begin(115200);
    IrSender.begin(IR_SEND_PIN, DISABLE_LED_FEEDBACK);
    //pinMode(ONBOARD_LED,OUTPUT);
    //input sensor pins pulled up for wire break safety
    pinMode(UPPER_LIMIT_SENSOR_PIN, INPUT_PULLUP); //nc switch, triggered when input high
    pinMode(LOWER_LIMIT_SENSER_PIN, INPUT_PULLUP); //nc switch, triggered when input high
    pinMode(UP_BUTTON_PIN, INPUT_PULLUP); //no switch, triggered when input low
    pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP); //no switch, triggered when input low
    pinMode(STOP_BUTTON_PIN, INPUT_PULLUP); //nc switch, triggered when input high
    setupOTA();
    Serial.println("Setup complete");
}
void loop() {
    ArduinoOTA.handle();
    checkButtons();
    checkLimitSwitches();

    if (state == up) {
        //send up ir
        IrSender.sendNEC(0x0, 0x95, 1);
        //play up anim
    } else if (state == down) {
        //send down ir
        IrSender.sendNEC(0x0, 0x99, 1);
        //play down anim
    }


    if (millis() - lastLedUpdate >= 50) {
        //state = down;
        updateLeds();
        lastLedUpdate = millis();
    }

}

void updateLeds() {

    if (state == up) {
        pixels.clear();
        pixels.setBrightness(100);
        for(int i=0; i<swipeLen; i++) {
            pixels.setPixelColor(swipePos - i, pixels.Color(0, 0, 255 - (255/swipeLen)*i, 0));
            pixels.show();
        }
        swipePos++;
        if (swipePos > NUM_LED + swipeLen) {
            swipePos = 0;
        }
    } else if (state == down) {
        pixels.clear();
        pixels.setBrightness(100);
        for(int i=0; i<swipeLen; i++) {
            pixels.setPixelColor(swipePos + i, pixels.Color(255 - (255/swipeLen)*i, 0, 0, 0));
            pixels.show();
        }
        swipePos--;
        if (swipePos < -swipeLen) {
            swipePos = NUM_LED;
        }
    } else {
        for(int i=0; i<NUM_LED; i++) {
            pixels.setPixelColor(i, pixels.Color(0, 255, 0, 0));
            pixels.show();
            pixels.setBrightness(1);
        }
        swipePos = 0;
    }

}

void checkButtons() {
    int bUP = digitalRead(UP_BUTTON_PIN);
    int bDown = digitalRead(DOWN_BUTTON_PIN);
    int bStop = digitalRead(STOP_BUTTON_PIN);


    if (bStop) {
        state = stopped;
        return;
    }
    if (state != stopped) {
        return;
    }
    if (bUP == HIGH && bDown == LOW) {
        state = down;
    } else if (bDown == HIGH && bUP == LOW) {
        state = up;
    } else {
        //TODO: print error, both buttons pressed (maybe special command?)
    }
}

void checkLimitSwitches() {
    int limUp = digitalRead(UPPER_LIMIT_SENSOR_PIN);
    int limLow = digitalRead(LOWER_LIMIT_SENSER_PIN);

    if (false){
        Serial.print("limUp = ");
        Serial.print(limUp);
        Serial.print("limLow = ");
        Serial.print(limLow);
        Serial.println();
    }
    //both sensors high -> invalid state
    if (limUp == HIGH && limLow == HIGH) {
        //TODO: print sensor error
        state = stopped;
        return;
    }

    if (state != stopped) {
        if (limUp || limLow) {
            state = stopped;
        }
    }
}