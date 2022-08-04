//
// Created by chriss on 02.08.22.
//
#pragma once

#define SEND_PWM_BY_TIMER

#include <IRremote.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "wifiCreds.h"

//enable Serial stream
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

#define UPPER_LIMIT_SENSOR_PIN 34
#define LOWER_LIMIT_SENSER_PIN 35
#define UP_BUTTON_PIN 25
#define STOP_BUTTON_PIN 26
#define DOWN_BUTTON_PIN 27
#define IR_SEND_PIN     5
#define ALED_PIN        13

#define NUM_LED     11

#define ota


void checkLimitSwitches();
void checkButtons();

void irSending();

TaskHandle_t send_ir_task;

enum state {
    up,
    down,
    stopped
};

TaskHandle_t Task1;

int lastLedUpdate;

state state = stopped;
u_int8_t swipeLen = 5;
int16_t swipePos = 0;
bool connected = true;

void send_ir_f(void *param){
    delay(500);
    while (true) {
        if (state == up) {
            //send up ir
            IrSender.sendNEC(0x0, 0x95, 1);
        } else if (state == down) {
            //send down ir
            IrSender.sendNEC(0x0, 0x99, 1);
        }
        yield();
    }
}


void setupOTA(){
    Serial << "Attempting to connect to " << ssid;
    Serial.println();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! No OTA available...");
        delay(2500);
        connected = false;
        break;
        //ESP.restart();
    }

    if (connected) {

        // Port defaults to 3232
        // ArduinoOTA.setPort(3232);

        // Hostname defaults to esp3232-[MAC]
        ArduinoOTA.setHostname("garagen-oeffner");

        // No authentication by default
        // ArduinoOTA.setPassword("admin");

        // Password can be set with it's md5 value as well
        // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
        // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

        ArduinoOTA
                .onStart([]() {
                    String type;
                    if (ArduinoOTA.getCommand() == U_FLASH)
                        type = "sketch";
                    else // U_SPIFFS
                        type = "filesystem";

                    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                    Serial.println("Start updating " + type);
                })
                .onEnd([]() {
                    Serial.println("\nEnd");
                })
                .onProgress([](unsigned int progress, unsigned int total) {
                    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
                })
                .onError([](ota_error_t error) {
                    Serial.printf("Error[%u]: ", error);
                    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
                    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
                    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
                    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
                    else if (error == OTA_END_ERROR) Serial.println("End Failed");
                });

        ArduinoOTA.begin();

        Serial.println("Connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
}



/*
 * Up:
    Protocol=NEC Address=0x0 Command=0x95 Raw-Data=0x6A95FF00 32 bits LSB first
    Send with: IrSender.sendNEC(0x0, 0x95, <numberOfRepeats>);
    rawData[68]:
     -2122700
     +9050,-4450
     + 600,- 550 + 600,- 550 + 550,- 600 + 500,- 600
     + 600,- 550 + 550,- 550 + 600,- 550 + 550,- 550
     + 600,-1650 + 600,-1700 + 550,-1700 + 550,-1700
     + 550,-1700 + 550,-1700 + 550,-1700 + 550,-1750
     + 550,-1700 + 550,- 600 + 500,-1750 + 550,- 550
     + 550,-1700 + 550,- 600 + 550,- 550 + 550,-1750
     + 500,- 600 + 600,-1700 + 500,- 600 + 550,-1700
     + 550,- 600 + 550,-1700 + 550,-1700 + 550,- 600
     + 500
    Down:

    Protocol=NEC Address=0x0 Command=0x99 Raw-Data=0x6699FF00 32 bits LSB first
    Send with: IrSender.sendNEC(0x0, 0x99, <numberOfRepeats>);
    rawData[68]:
     -389100
     +9050,-4500
     + 550,- 550 + 600,- 550 + 550,- 600 + 550,- 550
     + 600,- 550 + 550,- 600 + 550,- 550 + 550,- 600
     + 550,-1700 + 550,-1700 + 550,-1700 + 550,-1700
     + 600,-1650 + 550,-1700 + 600,-1700 + 550,-1700
     + 550,-1700 + 550,- 600 + 500,- 600 + 550,-1700
     + 550,-1700 + 600,- 550 + 550,- 550 + 600,-1700
     + 500,- 600 + 550,-1750 + 500,-1750 + 550,- 550
     + 550,- 600 + 550,-1700 + 550,-1700 + 550,- 600
     + 500
 */