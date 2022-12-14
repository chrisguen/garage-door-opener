#include <Arduino.h>
#include "main.h"

Adafruit_NeoPixel pixels(NUM_LED, ALED_PIN, NEO_GRBW + NEO_KHZ800);

void updateLeds();

void setup() {
    delay(200);
    pixels.setBrightness(100);
    lastLedUpdate = millis() + LED_UPDATE_INTERVAL;
    Serial.begin(115200);
    IrSender.begin(IR_SEND_PIN, DISABLE_LED_FEEDBACK);
    //pinMode(ONBOARD_LED,OUTPUT);
    //input sensor pins pulled up for wire break safety
    pinMode(UPPER_LIMIT_SENSOR_PIN, INPUT_PULLUP); //nc switch, triggered when input high
    pinMode(LOWER_LIMIT_SENSER_PIN, INPUT_PULLUP); //nc switch, triggered when input high
    pinMode(UP_BUTTON_PIN, INPUT_PULLUP); //no switch, triggered when input low
    pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP); //no switch, triggered when input low
    pinMode(STOP_BUTTON_PIN, INPUT_PULLUP); //nc switch, triggered when input high

    xTaskCreatePinnedToCore(
            send_ir_f,     /* Task function. */
            "Send IR code",    /* name of task. */
            2000,           /* Stack size of task */
            NULL,           /* parameter of the task */
            tskIDLE_PRIORITY,              /* priority of the task */
            &send_ir_task, /* Task handle to keep track of created task */
            0);

#ifdef ota
    setupOTA();
#endif
    Serial.print("Main Task running on core ");
    Serial.println(xPortGetCoreID());
    Serial.println("Setup complete");
}


void loop() {
#ifdef ota
    if (connected)ArduinoOTA.handle();
#endif
    checkButtons();
    checkLimitSwitches();

    if ((long)millis() - lastLedUpdate >= 0) {
        lastLedUpdate = millis() + LED_UPDATE_INTERVAL;
        updateLeds();
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

    if (bUP == HIGH && bDown == LOW) {
        state = down;
        swipePos = NUM_LED;
    } else if (bDown == HIGH && bUP == LOW) {
        state = up;
    } else {
        //TODO: print error, both buttons pressed (maybe special command?)
    }
}

void checkLimitSwitches() {
    int limUp = digitalRead(UPPER_LIMIT_SENSOR_PIN);
    int limLow = digitalRead(LOWER_LIMIT_SENSER_PIN);

    //both sensors high -> invalid state
    if (limUp == HIGH && limLow == HIGH) {
        //TODO: print sensor error
        state = stopped;
        return;
    }

    if (state == up && limLow) {
        return;
    }
    if (state == down && limUp) {
        return;
    }
    if (state == down && limLow) {
        state = stopped;
        return;
    }
    if (state == up && limUp) {
        state = stopped;
        return;
    }
}

