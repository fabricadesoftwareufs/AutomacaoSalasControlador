#include "Config.h"
#include "Hardware.h"

int __id;
String __uuid;
String __token;
int __typeDisp;
String __characteristic;
BLEAdvertisedDevice* __bleAdvertisedDevice;


Hardware::Hardware(){}

void Hardware::restart()
{   
    Config config;
    if (config.isDebug())
    {
        Serial.println("[Hardware] Reiniciando o dispositivo.......");
    }

    delay(2000);
    ESP.restart();
}

String Hardware::getMacAddress() const 
{
    Config config;
    String mac = WiFi.macAddress();
    mac.toUpperCase();
    if (config.isDebug())
    {
        Serial.println("[Hardware] MAC: " + mac);
    }

    return mac;
}

void Hardware::setId(int id)
{
  __id = id;
}

void Hardware::setUuid(String uuid)
{
  __uuid = uuid;
}

void Hardware::setToken(String token)
{
  __token = token;
}
void Hardware::setTypeDisp(int type)
{
    __typeDisp = type;
}

void Hardware::setCharacteristic(String characteristic)
{
    __characteristic = characteristic;
}

void Hardware::setBLEAdvertisedDevice(BLEAdvertisedDevice *bleAdvertisedDevice)
{
    __bleAdvertisedDevice = bleAdvertisedDevice;
}

int    Hardware::getId() const 
{
  return __id;
} 

String Hardware::getUuid() const 
{
  return __uuid;
} 

String Hardware::getToken() const 
{
  return __token;
}

int Hardware::getTypeDisp()
{
    return __typeDisp;
}

String Hardware::getCharacteristic()
{
    return __characteristic;
}

BLEAddress Hardware::getMacAddressAdvertisedDevice()
{
  return Hardware::getBLEAdvertisedDevice()->getAddress();
}

BLEAdvertisedDevice* Hardware::getBLEAdvertisedDevice()
{
    return __bleAdvertisedDevice;
}
