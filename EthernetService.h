#ifndef ETHERNET_SERVICE_H
#define ETHERNET_SERVICE_H

#include <ETH.h>

// Definições das pinagens e configurações
#define ETH_CLK_MODE                    ETH_CLOCK_GPIO17_OUT
#define ETH_ADDR                        0
#define ETH_TYPE                        ETH_PHY_LAN8720
#define ETH_RESET_PIN                   5
#define ETH_MDC_PIN                     23
#define ETH_MDIO_PIN                    18
#define SD_MISO_PIN                     2
#define SD_MOSI_PIN                     15
#define SD_SCLK_PIN                     14
#define SD_CS_PIN                       13
class EthernetService {
public:
    void init();
    bool isConnected();
private:
    bool eth_connected = false;
};

#endif // ETHERNET_SERVICE_H
