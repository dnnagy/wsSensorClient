// websocketSensor.ino
// Created by Nagy Daniel on 2017. jan. 28.
// simple client for sending APDS-9960 data to a server via websocket.

//Include for APDS-9960 sensor.
#include <Wire.h>
#include <SparkFun_APDS9960.h> //This sketch uses changes by me https://github.com/kommancs96/SparkFun_APDS-9960_Sensor_Arduino_Library

#include <AsyncPrinter.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncTCPbuffer.h>
#include <SyncClient.h>

#include <ESP8266WiFi.h>       //https://github.com/esp8266/Arduino


//For configuring WiFi
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>       //https://github.com/tzapu/WiFiManager
#include <WebSocketsClient.h>  //https://github.com/Links2004/arduinoWebSockets/tree/async
#include <Hash.h>

//Client configuration
#include "client_config.h"


// Pin definitions for nodeMCU
static const uint8_t D0 = 16;
static const uint8_t D1 = 5; // I2C Bus SCL (clock)
static const uint8_t D2 = 4; // I2C Bus SDA (data)
static const uint8_t D3 = 0;
static const uint8_t D4 = 2; // Same as "LED_BUILTIN", but inverted logic
static const uint8_t D5 = 14; // SPI Bus SCK (clock)
static const uint8_t D6 = 12; // SPI Bus MISO
static const uint8_t D7 = 13; // SPI Bus MOSI
static const uint8_t D8 = 15; // SPI Bus SS (CS)
static const uint8_t D9 = 3; // RX0 (Serial console)
static const uint8_t D10 = 1; // TX0 (Serial console)

//Initialize ws client
WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {
        switch(type) {
        case WStype_DISCONNECTED: {
                Serial.printf("[WSc] Disconnected!\n");
                break;
        }
        case WStype_CONNECTED: {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);
                break;
        }
        case WStype_TEXT: {
                Serial.printf("[WSc] get text: %s\n", payload);
                break;
        }
        case WStype_BIN: {
                Serial.printf("[WSc] get binary lenght: %u\n", lenght);
                break;
        }
        }
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
        Serial.println("Entered config mode");
        Serial.println(WiFi.softAPIP());

        //if you used auto generated SSID, print it
        Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
        Serial.begin(115200);
        /* Wifi debug messages */
        Serial.setDebugOutput(true);

        //WiFiManager
        //Local intialization. Once its business is done, there is no need to keep it around
        WiFiManager wifiManager;

//reset settings - for testing
#if WIFI_RESET
        wifiManager.resetSettings();
#endif

        //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
        wifiManager.setAPCallback(configModeCallback);


        //tries to connect to last known settings
        //if it does not connect it starts an access point with the specified name
        //here  "AutoConnectAP" with password "password"
        //and goes into a blocking loop awaiting configuration
        if (!wifiManager.autoConnect(WIFI_STA_NAME, WIFI_STA_PASSWD)) {
                Serial.println("failed to connect, we should reset as see if it connects");
                delay(3000);
                ESP.reset();
                delay(5000);
        }

        webSocket.begin(BACKEND_HOST, BACKEND_WS_PORT);
        webSocket.onEvent(webSocketEvent);
}
