
#ifndef Config_h
#define Config_h

#include "Enums.h"
#include <Arduino.h>
#ifdef _WIN32
    #include <String> // Para Windows
    #include <Vector>
#else
    #include <string.h> // Para Linux
    #include <vector>
#endif

#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <EmonLib.h>
#include <ArduinoJson.h>
#include "Controller.h"
#include "WiFiService.h"
#include "HTTP.h"
#include "Structs.h"
#include "HTTPService.h"
#include "Hardware.h"
#include "DHT.h"
#include "BLEServerService.h"
#include "AwaitHttpService.h"
#include "EnvironmentVariablesService.h"
#include "UtilsService.h"
#include <mutex>

class Config 
{
    private :
        String __tokenApp;
        String __url;
        String __ssid;
        String __password;
        int    __ledStatus;
        int    __basetime;
        bool   __debug;
        int    __type;
        int    __route;
        int    __wifiFailAttempts;
        int    __commandSendAttempts;
        int    __timesToHasOne;
        static std::mutex __bleActuatorMutex;
        static std::mutex __envVariablesMutex;

    public : 
        
        Config();
        
        String getTokenApp();
        String getUrl();
        String getSSID();
        String getPassword();
        int    getType();
        int    getBaseTime();
        int    getLedStatus();
        bool   isDebug();
        int    getRoute();        
        int    getWifiFailAttempts();
        int    getCommandSendAttempts();
        int    getTimesToHasOne();
        void   lock();
        void   unlock();
        void   lockEnvVariablesMutex();
        void   unlockEnvVariablesMutex();
};

#endif
