#include "EthernetService.h"
#include <Arduino.h>

static bool eth_connected = false;

void ETH_power_enable(bool enable)
{
    pinMode(-1, OUTPUT);
    if (enable) {
        digitalWrite(-1, HIGH);
    } else {
        digitalWrite(-1, LOW);
    }
}

void EthernetService::init() {
    ETH.begin(ETH_ADDR, ETH_RESET_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
    ETH.setHostname("LilyGO-POE");
    ETH_power_enable(true);

    if (!ETH.linkUp()) {
        Serial.println("Aguardando conexão Ethernet...");
        delay(10000);
    //     if ()
    // }
    //   Serial.println("Falha ao obter conexão Ethernet");
    //   Serial.println("Ethernet conectada!");
    //   eth_connected = true;
    //   Serial.print(F("Endereço IP: "));
       Serial.println(ETH.localIP());
    }
}

bool EthernetService::isConnected() {
    eth_connected = ETH.linkUp();
    return eth_connected;
}