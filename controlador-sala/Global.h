#ifndef GlobalESP_h
#define GlobalESP_h

#include <String>
#include <IRremoteESP8266.h>
#include <IRsend.h>
extern String COMMAND;
extern IRsend irsend;  // Set the GPIO to be used to sending the message.
extern bool SEND_DATA;

// Ambiente em aula  
extern bool IN_CLASS;

// Mensagem de retorno task http  
extern bool HTTP_RECEIVED_DATA;
extern String HTTP_MESSAGE;

// Mensagem de retorno task variaveis de ambiente
extern bool ENV_RECEIVED_DATA;
extern String ENV_MESSAGE;

extern bool HTTP_REQUEST;
extern bool ENV_REQUEST;

#endif