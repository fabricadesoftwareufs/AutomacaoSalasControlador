#ifndef ClientSocketService_h
#define ClientSocketService_h

#include "Config.h"

#define CONDICIONADOR  "CONDICIONADOR"
#define LUZES "LUZES"
#define ATUALIZAR "ATUALIZAR_HORARIOS"


class ClientSocketService 
{
  private: 
    static bool __messageReturned;
    static String __message;
    static WiFiServer __server;
    static void awaitsReturn();
    static MonitoringRequest deserealizeObject(String payload);

  public: 
    ClientSocketService();

    WiFiClient getClient();
    void setClient(WiFiClient client);

    String getMessage();
    void setMessage(String message);

    bool getMessageReturned();
    void setMessageReturned(bool messageReturned);

    static bool connectToActuator(String uuidDevice); 

    void initServer();
    
    //Task
    void serverListener();
    static void startTaskWebSocketImpl(void* _this);
    void startTaskWebSocket();

    String getMessageToSend(MonitoringRequest request);
};

#endif
