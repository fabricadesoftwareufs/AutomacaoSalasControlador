#include "Config.h"
#include "WiFiService.h"

WiFiService::WiFiService(){}
Config config;

void WiFiService::connect()  
{   
    delay(4000);
    
    int attempt = 0;
    WiFi.begin(config.getSSID().c_str(), config.getPassword().c_str());
    while (WiFi.status() != WL_CONNECTED && attempt < config.getWifiFailAttempts())
    {
        digitalWrite(config.getLedStatus(), HIGH);
        delay(1000);
        digitalWrite(config.getLedStatus(), LOW); 
        attempt += 1;
        if (config.isDebug())
        {
            Serial.println("==================================");
            Serial.println("[WiFiService] Conectando ao Wifi");
            Serial.println("[WiFiService] Tentativa: " + String(attempt));
            Serial.println("[WiFiService] SSID: " + config.getSSID());
            Serial.println("[WiFiService] Password: " + config.getPassword());
        }
    }

    if(WiFi.status() != WL_CONNECTED && attempt == config.getWifiFailAttempts()) {
      Serial.println("[WiFiService] Excedido o número de tentativas de conexão ao Wifi: " + config.getWifiFailAttempts());
      Serial.println("[WiFiService] Reiniciando ESP...");
      ESP.restart();
    }

    digitalWrite(config.getLedStatus(), HIGH);
    
    if (WiFi.status() == WL_CONNECTED && config.isDebug())
    {
        IPAddress ip = WiFi.localIP();
        Serial.println("==================================");
        Serial.println("[WiFiService] Conectado a rede: " + config.getSSID());
        Serial.print("[WiFiService] Ip: ");
        Serial.println(ip);
    }
   
    return;
}

void WiFiService::disconnect()  
{
    WiFi.disconnect();
    digitalWrite(config.getLedStatus(), LOW); 

    if (config.isDebug())
    {
        Serial.println("==================================");
        Serial.println("[WiFiService] Desconectado da rede: " + config.getSSID());
    }
    
    return;
}
