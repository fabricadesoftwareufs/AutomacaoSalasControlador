#include "Config.h"
#include "EthernetService.h"
#include <SPI.h>
#include <ETH.h>
#include "utilities.h"
// #if (RH_PLATFORM == RH_PLATFORM_ESP8266)
//     // interrupt handler and related code must be in RAM on ESP8266,
//     // according to issue #46.
//     #define INTERRUPT_ATTR ICACHE_RAM_ATTR
// #elif (RH_PLATFORM == RH_PLATFORM_ESP32)
//     #define INTERRUPT_ATTR IRAM_ATTR 
// #else
//     #define INTERRUPT_ATTR
// #endif

BLEServerService* bleConfig;

HardwareRecord hardware;
Controller controller;
EthernetService ethernetService;
WiFiService wiFiService;

void setup() {
    Serial.begin(115200);
    bool init = false;

    ethernetService.init();

    if (ethernetService.isConnected()) {
        Serial.println("Ethernet conectado");
        //wiFiService.disconnect();

    } else {
        Serial.println("Ethernet não conectado, conectando ao Wi-Fi...");
        wiFiService.connect();
    }

    do {
        if (controller.start(hardware)) {
            if (controller.registerHardware(hardware)) {
                controller.setHardwareConfig(hardware);
                controller.fillHardwares(hardware);

                if (controller.loadedDevices())
                    init = true;
            }
        }
    } while (!init);

    // Configure BLE Service
    controller.setupBLEServer();
    controller.startBLETaskServer();

    // Configure Http Service
    controller.startTaskHttp();

    // Configure Environment Variables Service
    controller.setupEnvironmentVariables();
}

void loop() {
    delay(1000);
    controller.environmentVariablesContinuousValidation();
}
