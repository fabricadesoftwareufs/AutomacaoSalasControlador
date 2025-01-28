#include "EquipmentService.h"

bool __lightOn;
EnergyMonitor __sct13;

EquipmentService::EquipmentService() {  
  __sct13.current(pinSCT, 6.0606); 
}

bool EquipmentService::getLightOn() {
  return __lightOn;
}

void EquipmentService::setLightOn(bool light) {
  __lightOn = light;
}

EnergyMonitor EquipmentService::geSct() {
  return __sct13;
}

void EquipmentService::setSct(EnergyMonitor sct) {
  __sct13 = sct;
}

/*
   <descricao> Obtem nome do dispositivo ou os codigos IR neviados na requisicao do servidor  <descricao/>
   <parametros> data: codigos IR recebidos na requisicao do servidor <parametros/>
   <parametros> separator: caracter chave para realizar o 'split' <parametros/>
   <parametros> index: identificar que diz se quem chama quer receber o nome do dispositivo ou os codigos IR <parametros/>
   <retorno> string com nome do dispotivo recebido na requisicao ou os codigos IR <retorno/>
*/
String EquipmentService::SplitGetIndex(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {
    0,
    -1
  };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


/*
   <descricao>   <descricao/>
   <parametros> data: codigos IR recebidos na requisicao do servidor <parametros/>
   <retorno>  <retorno/>
*/
void EquipmentService::SplitIrComands(String codigoString, vector <int> &codigo) {

  String temp = "";
  for (int i = 0; i < codigoString.length(); i++)
  {
    if (codigoString.charAt(i) == ',')
    {
      codigo.push_back(temp.toInt());
      temp = "";
    }
    else
    {
      if (codigoString.charAt(i) != ';' || codigoString.charAt(i) != ' ')
        temp += codigoString.charAt(i);
    }
  }
  codigo.push_back(temp.toInt());
}


/*
   <descricao>   <descricao/>
   <parametros> data: codigos IR recebidos na requisicao do servidor <parametros/>
   <retorno>  <retorno/>
*/
void EquipmentService::SendIrComand(vector<int> codigo) {
  int k = 0;
  uint16_t rawData[codigo.size()];
  for (int el : codigo)
  {
    rawData[k] = (uint16_t) el;
    k++;
  }

  Serial.println("==================================");
  Serial.println("[EquipmentService] Command Size: " +  String(codigo.size()));
  irsend.sendRaw(rawData, codigo.size(), 38); // envia comando IR para o equipamento
  delay(1000);
}

/*
   <descricao>   <descricao/>
   <parametros> data: codigos IR recebidos na requisicao do servidor <parametros/>
   <retorno>  <retorno/>
*/
bool EquipmentService::checkIrms() {

  double Irms = __sct13.calcIrms(1480); // Calcula o valor da Corrente
  int potencia = Irms * tensao; // Calcula o valor da Potencia Instantanea

  if (Irms > 2) // se a corrente for maior que (valor de Ampere considerado ligado, é enviado a resposta para aplicação que o sensor está ligado
    return true;

  return false;
}


/*
   <descricao> Executa o comando de ligar luzes e envia o status do monitoramento pra o servidor além de gravar a operação em log <descricao/>
*/
void EquipmentService::checkOperationLights(String msg) {
  if (msg == "ON")
    turnOnLights();
  else
    turnOffLights();
}

/*
   <descricao> Executa o comando de ligar luzes e envia o status do monitoramento pra o servidor além de gravar a operação em log <descricao/>
*/
void EquipmentService::turnOnLights() {
  /*
     Ligando luzes
  */
  Serial.println("LIGANDO");

  __lightOn = true;
  digitalWrite(RELE, HIGH);
}

/*
   <descricao> Executa o comando de desligar luzes e envia o status do monitoramento pra o servidor além de gravar a operação em log <descricao/>
*/
void EquipmentService::turnOffLights() {
  /*
     Desligando luzes
  */
  Serial.println("DESLIGANDO");

  __lightOn = false;
  digitalWrite(RELE, LOW);
}

String EquipmentService::executeActionFromController(String data) {

  Serial.println("==================================");
  Serial.print("[EquipmentService] executar comando recebidos do controlador : ");

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, data);

  if (error)
  {
    Serial.println("==================================");
    Serial.println("[EquipmentService] Erro ao deserializar objeto ");

    return "ERROR";
  }

  String type = doc["type"].as<String>();
  String command = doc["command"].as<String>();
  String state = doc["state"].as<String>();


  if (type == NULL)
  {
    return "ERROR";
  }
  else if (type.equals("LZ"))
  {
    checkOperationLights(state);
    return state.equals("ON") ? LZ_ON : LZ_OFF;
  }
  else if (type.equals("AC"))
  {
    Serial.print("[EquipmentService] Command: " + String(command));
    Serial.println("==================================");
    return executeActionIntoConditioner(command, state);
  }

  return "ERROR";
}

String EquipmentService::executeActionIntoConditioner(String command, String state){
  
  Config config;
  vector <int> codigo;
  SplitIrComands(command, codigo);
  int attempt = 0;
  bool isSuccessful, isOn;

  do
  {
      Serial.println("[EquipmentService] Enviando comando, tentativa: " + attempt);

      SendIrComand(codigo);
      isOn = checkIrms();

      isSuccessful = (isOn == state.equals("ON") ? AC_ON : AC_OFF);
  
      attempt++;

  } while (!isSuccessful && attempt < config.getCommandSendAttempts());

  Serial.println("[EquipmentService] Comando finalizado, Sucesso: " + isSuccessful);

  return isOn ? AC_ON : AC_OFF;
}
