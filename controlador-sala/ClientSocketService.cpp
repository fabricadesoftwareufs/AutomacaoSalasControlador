#include "Config.h"
#include "ClientSocketService.h"

Config configuration;
BLEServerService* __bleClientConfiguration; 
HTTPService __httpClientService;
EnvironmentVariablesService __env;
UtilsService __utilsClient;

ClientSocketService::ClientSocketService() {}

bool ClientSocketService::__messageReturned = false;
String ClientSocketService::__message = "";
WiFiServer ClientSocketService::__server(8088);

void ClientSocketService::initServer() {
      __server.begin();
}

String ClientSocketService::getMessage() {
    return  __message;
}

void ClientSocketService::setMessage(String message) {
    __message = message;
}

bool ClientSocketService::getMessageReturned() {
    return  __messageReturned;
}

void ClientSocketService::setMessageReturned(bool messageReturned) {
    __messageReturned = messageReturned;
}

/*
 * <descricao> Ouve requisicoes do cliente conecta via socket <descricao/>
 */
void ClientSocketService::serverListener() {

    WiFiClient client;    
    
    while(true) {          
      /* 
       * ouvindo o cliente 
       */
      client = __server.available();
  
      if (client) {
  
        /*
         * Checando se o cliente está conectando ao server
         */
        while (client.connected()) {
  
          if (client.available()) {
            String && msg = client.readStringUntil('\n');
            
            if (configuration.isDebug())
            {
                Serial.println("=======================================");
                Serial.println("[ClientSocketService] mensagem recebida");
                Serial.println("[ClientSocketService] mensagem: " + msg);
            }

            MonitoringRequest request = deserealizeObject(msg);
            
            if (configuration.isDebug())
            {
                Serial.println("=======================================");
                Serial.println("[ClientSocketService] type: " + request.type);
                Serial.println("[ClientSocketService] code: " + request.code);
                Serial.println("[ClientSocketService] uuid: " + request.uuid);
            }
            
            if (request.type == CONDICIONADOR || request.type == LUZES) { 

                //__bleClientConfiguration->setReceivedRequest(true);

                bool dispConnected = connectToActuator(request.uuid);
                
                if(dispConnected)
                {
                  String payload = getMessageToSend(request);
                  __bleClientConfiguration->sendMessageToActuator(payload);

                  awaitsReturn();

                  __bleClientConfiguration->disconnectToActuator();
                }
                
                //__bleClientConfiguration->setReceivedRequest(false);

                if(__messageReturned)   
                  client.println(__message);
                else
                  client.println("NOT-AVALIABLE");

                __utilsClient.updateMonitoring(__message);

                if (configuration.isDebug())
                {
                  Serial.println("==================================");
                  Serial.println("[ClientSocketService] Resposta BLE");
                  Serial.println("[ClientSocketService] recebeu retorno: " + __messageReturned);
                  Serial.println("[ClientSocketService] mensagem: " + __message);
                }

                __messageReturned = false;
                __message = "";  

            }  else if(request.type == ATUALIZAR) {
                  
                __env.setReservations(__httpClientService.getReservationsToday());

                client.println("OK");
            }
          }  
          delay(100);
        }
      }
      delay(500);
    }
}

MonitoringRequest ClientSocketService::deserealizeObject(String payload)
{
    MonitoringRequest request;

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);
    
    request.type = doc["type"].as<const char *>();
    request.code = doc["code"].as<const char *>();
    request.uuid = doc["uuid"].as<const char *>();
    request.acting = doc["acting"].as<const char *>();
    
    return request;
}


bool ClientSocketService::connectToActuator(String uuidDevice) 
{
  bool deviceConnected = false;
  int i = 0;
  int count = 5;
            
  do
  { 
    i++;
    
    if (configuration.isDebug())
    {
      Serial.print("[ClientSocketService]: attempt number: ");
      Serial.println(i);
    }
    
    deviceConnected = __bleClientConfiguration->connectToActuator(uuidDevice);
    
    if(deviceConnected)
      break;
      
  } while(i < count);

  if( i >= count && !deviceConnected)
      Serial.println("[ClientSocketService]: device not found");

  return deviceConnected;
}

void ClientSocketService::awaitsReturn()
{
  
  unsigned long tempoLimite = millis() + 15000;
  while(millis() <= tempoLimite && !__messageReturned)
  { 
      delay(1000);
      if (configuration.isDebug())
      {    
        Serial.print("[ClientSocketService] TIME AWAITS: ");
        Serial.println(millis());
      }
  }    
}

void ClientSocketService::startTaskWebSocketImpl(void* _this)
{
    ClientSocketService* socketService = (ClientSocketService*)_this;
    socketService->serverListener();
}

void ClientSocketService::startTaskWebSocket()
{
  xTaskCreate(this->startTaskWebSocketImpl, "serverListener", 8192, this, 5, NULL);
}

String ClientSocketService::getMessageToSend(MonitoringRequest request)
{
    String typeEquipament = "", state = "", command = "null";

    if(request.type == LUZES)
        typeEquipament = "LZ";
    else
    {
        typeEquipament = "AC";
        command = request.code;
    }

    if(request.acting == "True")
        state = "ON";
    else
        state = "OFF";

    return __utilsClient.mountPayload(typeEquipament, state, command);
}
