#include "Config.h"
#include "HTTP.h"

HTTP::HTTP(){}

String HTTP::request(String resource, String type, String params) const{
    
    Config config;
    WiFiService wifi;
    int httpCode = 0;
    String response = "";
    String url = config.getUrl(); 
    
    resource.trim();
    resource.toLowerCase();
    
    type.trim();
    type.toLowerCase();
  
    if ( !(type.compareTo("put") == 0) && !(type.compareTo("post") == 0) && !(type.compareTo("get") == 0)){
        if(config.isDebug()){
          Serial.println("==================================");  
          Serial.println("[HTTP] Tipo de requisição [" + type + "] inválido");
        }
        return "Tipo de requisição inválido";
    }
        
    if (WiFi.status() == WL_CONNECTED) {
      
        HTTPClient http;
      
        if(config.isDebug()){
            Serial.println("==================================");  
            Serial.println("[HTTP] Conectado");
        }
      
        url.concat(resource);
  
        if(config.isDebug()){
            Serial.println("==================================");  
            Serial.println("[HTTP] URL...: " + url);
            Serial.println("[HTTP] Tipo..: " + type);
            Serial.println("[HTTP] Params: " + params);
        }
    
        http.begin(url.c_str());
        if (type.compareTo("post") == 0){

            http.addHeader("Content-Type", "application/json");          
            httpCode = http.POST(params);
            Serial.println("[HTTP] Code: " + httpCode);

        }else if(type.compareTo("put") == 0){

            http.addHeader("Content-Type", "application/json");          
            httpCode = http.PUT(params);
            Serial.println("[HTTP] Code: " + httpCode);        
        
        }else if(type.compareTo("get") == 0){
            httpCode = http.GET();
        }

        if (httpCode > 0) { 
            if(httpCode == 204){
              response = "[NO_CONTENT]: ";
              response.concat(httpCode);
            }
            else {
              response = http.getString();
            }
        }
        else{
            response = "[ERROR]";
            response.concat(httpCode);
        }
      
        if(config.isDebug()){
            Serial.println("==================================");  
            Serial.println("[HTTP] Código: " + String(httpCode));
            Serial.println("[HTTP] Resposta: " + response);
        }
      
        http.end();
    }else{
      
        if(config.isDebug()){
            Serial.println("==================================");  
            Serial.println("[HTTP] Desconectado");
        }
      
    }

    if(config.isDebug()){
        Serial.println("==================================");  
        Serial.println("[HTTP] HTTP FINALIZADO: " + String(httpCode));
    }
    
    if(config.isDebug()){
        Serial.println("==================================");  
        Serial.println("[HTTP] WIFI DESCONECTADO: " + String(httpCode));
    }

    return response;
}
