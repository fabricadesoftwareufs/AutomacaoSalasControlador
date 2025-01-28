#include "Config.h"
#include "HTTPService.h"

const size_t CAPACITY = JSON_ARRAY_SIZE(12);

HTTPService::HTTPService() {}

void HTTPService::getInfoHardware(HardwareRecord &record)
{

    // hardware/MAC?api_token = TOKEN_BASE&tipo-hardware=TIPO

    Config config;
    Hardware hardware;
    HTTP http;
    String route;

    if (config.getRoute() == 1)
        route = "/hardware/";
    else
        route = "/hardwaredesala/info/";

    String routeService;
    String type = "GET";
    String params = "";
    String token = "594ac3eb82b5080393ad5c426f61c1ed5ac53f90e1abebc15316888cf1c8f5fe";
    String response;

    routeService.concat(route);
    routeService.concat(hardware.getMacAddress());
    routeService.concat("?token=");
    routeService.concat(token);
    routeService.concat("&tipo-hardware=");
    routeService.concat(config.getType());

    response = http.request(routeService, type, params);

    if (strstr(response.c_str(), "[ERROR]") == NULL && strstr(response.c_str(), "[NO_CONTENT]") == NULL)
    {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, response);

        if (error)
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.println("[HTTPService] Falha no parse JSON.......");
                Serial.println(error.f_str());
            }
            delay(5000);

            return;
        }

        if (doc["httpCode"].as<int>() == 200)
        {

            record.id = doc["result"]["id"].as<int>();
            record.token = doc["result"]["token"].as<char *>();
            record.uuid = doc["result"]["uuid"].as<char *>();
            record.salaId = doc["result"]["salaId"].as<int>();

            return;
        }
        else
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.print("[HTTPService] Mensagem: ");
                Serial.println(doc["message"].as<char *>());
            }
            return;
        }
    }
    else
    {
        return;
    }

    return;
}

bool HTTPService::registerHardware(HardwareRecord hardware)
{

    //hardware/register

    Config config;
    HTTP http;
    String route;

    if (config.getRoute() == 1)
        route = "/hardware/register";
    else
        route = "/hardwaredesala/register";

    String routeService;
    String type = "POST";
    String params = "";
    String response;

    String id = String(hardware.id);
    String tipo_hardware_id = String(config.getType()); 

    params.concat("{");
    params.concat("\"id\":" + id + ", ");
    params.concat("\"token\":\"" + hardware.token + "\", ");
    params.concat("\"tipo_hardware_id\": " + tipo_hardware_id);
    params.concat("}");

    routeService.concat(route);
    response = http.request(routeService, type, params);

    return true;
}

std::vector<struct HardwareRecord> HTTPService::getHardwares(struct HardwareRecord hardware)
{
    Config config;
    HTTP http;
    String route;
    std::vector<struct HardwareRecord> hardwares;

    if (config.getRoute() == 1)
        route = "/hardware/";
    else
        route = "/hardwaredesala/";

    String routeService;
    String type = "GET";
    String params = "";
    String response;

    String uuid = String(hardware.uuid);
    String token = "594ac3eb82b5080393ad5c426f61c1ed5ac53f90e1abebc15316888cf1c8f5fe";

    routeService.concat(route);
    routeService.concat(hardware.salaId);
    routeService.concat("/get-sensors-and-actuators");
    routeService.concat("?token=");
    routeService.concat(token);

    response = http.request(routeService, type, params);

    if (strstr(response.c_str(), "[ERROR]") == NULL && strstr(response.c_str(), "[NO_CONTENT]") == NULL)
    {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, response);
                         
        if (error)
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.println("[HTTPService] Falha no parse JSON.......");
                Serial.println(error.f_str());
            }
            delay(5000);

            return hardwares;
        }

        if (doc["httpCode"].as<int>() == 200)
        {

            JsonArray jsonSensors = doc["result"].as<JsonArray>();

            for (JsonVariant sensor : jsonSensors)
                hardwares.push_back(deserializeDevice(sensor));

        }
        else
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.print("[HTTPService] Mensagem: ");
                Serial.println(doc["message"].as<char *>());
            }
        }
    }

     if (config.isDebug())
    {
        Serial.println("==================================");
        Serial.print("[HTTPService] count hardwares: ");
        Serial.println(hardwares.size());
    }

    return hardwares;
}

/*
 * <descricao> Deserealiza objeto json e converte para a struct que armazena as reservas  <descricao/>
 */
struct HardwareRecord HTTPService::deserializeDevice(JsonVariant sensor) {
   
   struct HardwareRecord disp;

   disp.uuid = sensor["uuid"].as<String>();
   disp.typeHardwareId = sensor["tipoHardwareId"].as<int>();
   disp.typeEquipment = sensor["tipoEquipamento"].as<int>(); 
   return disp;
}

/*
 * <descricao> Deserealiza objeto json e converte para a struct que armazena a solicitacao  <descricao/>
 */
struct Solicitacao HTTPService::deserializeSolicitacao(int idSolicitacao, String payload) {
    
    struct Solicitacao solicitacao;
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, payload);

    solicitacao.id = idSolicitacao;    
    solicitacao.code = doc["code"].as<char *>();
    solicitacao.type = doc["type"].as<char *>();
    solicitacao.uuid = doc["uuid"].as<char *>();
    solicitacao.acting = doc["acting"].as<char *>();
 
   return solicitacao;
}

bool HTTPService::getMaster(struct HardwareRecord hardware, String &master)
{
    //hardware/{uuid}/get-master?token=TOKEN

    Config config;
    HTTP http;
    String route;

    if (config.getRoute() == 1)
        route = "/hardware/";
    else
        route = "/hardwaredesala/slave/";

    String routeService;
    String type = "GET";
    String params = "";
    String response;

    String uuid = String(hardware.uuid);
    String token = hardware.token;

    routeService.concat(route);
    routeService.concat(uuid);
    routeService.concat("/get-master");
    routeService.concat("?token=");
    routeService.concat(token);

    response = http.request(routeService, type, params);

    if (strstr(response.c_str(), "[ERROR]") == NULL && strstr(response.c_str(), "[NO_CONTENT]") == NULL)
    {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, response);

        if (error)
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.println("[HTTPService] Falha no parse JSON.......");
                Serial.println(error.f_str());
            }
            delay(5000);

            return false;
        }

        if (doc["httpCode"].as<int>() == 200)
        {
            master = doc["result"]["uuid"].as<char *>();

            return true;
        }
        else
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.print("[HTTPService] Mensagem: ");
                Serial.println(doc["message"].as<char *>());
            }
            return false;
        }
    }

    return false;
}

String HTTPService::getTime(String identifier)
{
    Config config;
    HTTP http;
    String route;

    if (config.getRoute() == 1)
        route = "";
    else
        route = "/Time/";

    String routeService;
    routeService.concat(route);
    routeService.concat(identifier);
  
    String response = http.request(routeService, "GET", "");

    if (strstr(response.c_str(), "[ERROR]") != NULL)
        return "";

    return response;
}


/*
 * <descricao> Realiza requisicao ao servidor para obter as reservas da semana para a sala deste dispositivo <descricao/>   
 */
std::vector<struct Reserva> HTTPService::getReservationsToday() {
    
    HTTP http;
    String route;
    Config config;
    EnvironmentVariablesService environment;
    std::vector<struct Reserva> reservas;
        
    if (config.getRoute() == 1)
        route = "";
    else
        route = "/HorarioSala/ReservasDeHojePorUuid/";

    String routeService;
    String type = "GET";
    String params = "";
    String uuid = environment.getHardware().uuid;

    routeService.concat(route);
    routeService.concat(uuid);

    String response = http.request(routeService, type, params);
    
    if (strstr(response.c_str(), "[ERROR]") == NULL && strstr(response.c_str(), "[NO_CONTENT]") == NULL)
    {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, response);

        if (error)
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.println("[HTTPService] Falha no parse JSON.......");
                Serial.println(error.f_str());
            }
            delay(5000);
        }

        if (doc["httpCode"].as<int>() == 200)
        {
            JsonArray jsonSensors = doc["result"].as<JsonArray>();

            for (JsonVariant object : jsonSensors)
            {
                reservas.push_back(deserializeReserve(object));
            }
        }
        else
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.print("[HTTPService] Mensagem: ");
                Serial.println(doc["message"].as<char *>());
            }
        }
    }

    if (config.isDebug())
    {
        Serial.println("==================================");
        Serial.print("[HTTPService] count reservations: ");
        Serial.println(reservas.size());
    }
    
    return reservas;
}


/*
 * <descricao> Deserealiza objeto json e converte para a struct que armazena as reservas  <descricao/>
 */
struct Reserva HTTPService::deserializeReserve(JsonVariant reserve) {
   
   struct Reserva res;

   res.id = reserve["id"].as<int>();
   res.usuarioId = reserve["id-usuario"].as<int>();
   res.salaId = reserve["id-sala"].as<int>();
   res.planejamento = reserve["planejamento"].as<int>();
   res.date = reserve["data"].as<String>();
   res.horarioInicio = reserve["horario-inicial"].as<String>();
   res.horarioFim = reserve["horario-final"].as<String>();
   res.situacao = reserve["status"].as<String>();
   res.objetivo = reserve["objective"].as<String>();  

   //if (config.isDebug())
   //{
        Serial.println("==================================");
        Serial.print("[HTTPService] horarioInicio: ");
        Serial.println(res.horarioInicio);
        Serial.print("[HTTPService] horarioFim: ");
        Serial.println(res.horarioFim);
   //}

   return res;
}

/*
 * <descricao> Obtem o estado atual do monitoramento da sala  <descricao/>
 * <retorno> Struct Monitoramento com os dados do monitoramento de acordo com o banco <retorno/>
 */
struct Monitoramento HTTPService::getMonitoringByIdSalaAndEquipamento(String tipoEquipamento) {

    HTTP http;
    String route;
    Config config;
    struct Monitoramento monitoramento = {0, false, 0};
    EnvironmentVariablesService environment;
        
    if (config.getRoute() == 1)
        route = "";
    else
        route = "/Monitoramento/obter-por-sala-tipo-equipamento/";

    String routeService;
    String type = "GET";
    String params = "";

    routeService.concat(route);
    routeService.concat(environment.getHardware().salaId);
    routeService.concat("/");
    routeService.concat(tipoEquipamento);

    String response = http.request(routeService, type, params);
                
    if (strstr(response.c_str(), "[ERROR]") == NULL && strstr(response.c_str(), "[NO_CONTENT]") == NULL)
    {
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, response);
        
        if (error)
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.println("[HTTPService] Falha no parse JSON.......");
                Serial.println(error.f_str());
            }
            delay(5000);
        }

        if (doc["httpCode"].as<int>() == 200)
        {
            monitoramento.id = doc["result"]["id"].as<int>();
            monitoramento.estado = doc["result"]["estado"].as<bool>();
            monitoramento.equipamentoId = doc["result"]["equipamentoId"].as<int>();
        }
        else
        {
            Serial.println(doc["httpCode"].as<int>());

            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.print("[HTTPService] Mensagem: ");
                Serial.println(doc["message"].as<char *>());
            }
        }
    }

    return monitoramento;
}


/*
 * <descricao> Atualiza a tabela Monitoramento do banco de dados com as atualizacoes feitas nos equipamentos pelo ESP  <descricao/>
 * <parametros> luzes: indica o ultimo estado das luzes (ligado/desligado) <parametros/>
 * <parametros> condicionador: indica o ultimo estado do ar condicionado (ligado/desligado) <parametros/>
 * <retorno> string com nome do dispotivo recebido na requisicao ou os codigos IR <retorno/>
 */
bool HTTPService::putMonitoring(struct Monitoramento monitoring) {

    Config config;
    HTTP http;
    String route;
    EnvironmentVariablesService environment;

    if (config.getRoute() == 1)
        route = "";
    else
        route = "/monitoramento";

    String routeService;
    String type = "PUT";
    String params = "";
    String response;

    String id               = String(monitoring.id);
    String estado           = String(monitoring.estado ? "true" : "false");
    String equipamento      = String(monitoring.equipamentoId);

    params.concat("{ ");
    params.concat("\"id\": "              + id      + ", ");
    params.concat("\"estado\": "          + estado  + ", ");
    params.concat("\"equipamentoId\": "   + equipamento + ", ");
    params.concat("}");

    routeService.concat(route);
    response = http.request(routeService, type, params);

    if (strstr(response.c_str(), "[ERROR]") == NULL && strstr(response.c_str(), "[NO_CONTENT]") == NULL)
    {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, response);

        if (error)
        {
                if (config.isDebug())
                {
                    Serial.println("==================================");
                    Serial.println("[HTTPService] Falha no parse JSON.......");
                    Serial.println(error.f_str());
                }
                delay(5000);
        }

        if (doc["httpCode"].as<int>() == 200)
            return true;
    }

    return false;
}

/*
 * <descricao> Obtem do servidor os codigos IR para ligar/desligar o arcondicionado <descricao/>
 * <parametros> operacao: operacao que deve ser consultados os codigos IR (ligar/desligar) <parametros/>
 * <retorno> lista de inteiros com os codigos IR solicitados <retorno/>
 */
String HTTPService::getComandosIrByIdSalaAndOperacao(String uuid) {

    Config config;
    HTTP http;
    String route;
    String codigo = "";
    EnvironmentVariablesService environment;

    if (config.getRoute() == 1)
        route = "";
    else
        route = "/infravermelho/CodigosPorUuid/";

    String routeService;
    String type = "GET";
    String params = "";

    routeService.concat(route);
    routeService.concat(uuid);
  
    String response = http.request(routeService, type, params);

    if (strstr(response.c_str(), "[ERROR]") == NULL && strstr(response.c_str(), "[NO_CONTENT]") == NULL)
    {
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, response);

        if (error)
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.println("[HTTPService] Falha no parse JSON.......");
                Serial.println(error.f_str());
            }
            delay(5000);
        }

        if (doc["httpCode"].as<int>() == 200)
        {
            JsonArray jsonSensors = doc["result"].as<JsonArray>();

            for (JsonVariant object : jsonSensors)
            {
                codigo = object["codigo"].as<String>();
            }
        }
        else
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.print("[HTTPService] Mensagem: ");
                Serial.println(doc["message"].as<char *>());
            }
        }
    }

    return codigo;
}

/**
 * <descricao> Obtém as solicitações em aberto para o hardware </descricao>
 * <parametros> tipo Tipo de Equipamento: LUZES, CONDICIONADOR</parametros>
 * <retorno> struct Solicitacao referente a solicitação existente para o hardware </retorno>
*/
struct std::vector<Solicitacao> HTTPService::getSolicitacao(String tipoEquipamento){
    
    HTTP http;
    Config config;
    std::vector<Solicitacao> solicitacao;
    EnvironmentVariablesService environment;

    String route = "/Solicitacao";
    String routeService;
    String type = "GET";
    String params = "";
    String idHardware = String(environment.getHardware().id);

    routeService.concat(route);
    routeService.concat("?idHardware=" + idHardware);
    routeService.concat("&tipo=" + tipoEquipamento);
    routeService.concat("&todosRegistros=false");
    
    String response = http.request(routeService, type, params);
    
    if (strstr(response.c_str(), "[ERROR]") == NULL && strstr(response.c_str(), "[NO_CONTENT]") == NULL)
    {
        DynamicJsonDocument doc(4096);
        DeserializationError error = deserializeJson(doc, response);
        
        if (error)
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.println("[HTTPService] Falha no parse JSON.......");
                Serial.println(error.f_str());
            }
            delay(5000);
        }



        if (doc["httpCode"].as<int>() == 200)
        {
            JsonArray solicitacoesJson = doc["result"].as<JsonArray>();

            for (JsonVariant sol : solicitacoesJson)
            {
                solicitacao.push_back(deserializeSolicitacao(sol["id"].as<int>(), sol["payload"].as<String>()));
            }
        }
        else
        {
            Serial.println(doc["httpCode"].as<int>());

            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.print("[HTTPService] Mensagem: ");
                Serial.println(doc["message"].as<char *>());
            }
        }
    }

    return solicitacao;
}

/**
 * <descricao> Finaliza a solicitacao </descricao>
 * <parametros> idSolicitacao Identificador da solicitação</parametros>
 * <retorno> bool Retorno da solicitação </retorno>
*/
bool HTTPService::putSolicitacao(int idSolicitacao) {

    Config config;
    HTTP http;
    String route;
    EnvironmentVariablesService environment;
    
    route = "/Solicitacao/finalizar-solicitacao-by-id/";

    String routeService;
    String type = "PUT";
    String response;
    String id = String(idSolicitacao);

    routeService.concat(route + id);
    response = http.request(routeService, type, "{}");

    if (strstr(response.c_str(), "[ERROR]") == NULL && strstr(response.c_str(), "[NO_CONTENT]") == NULL)
    {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, response);

        if (error)
        {
            if (config.isDebug())
            {
                Serial.println("==================================");
                Serial.println("[HTTPService] Falha no parse JSON.......");
                Serial.println(error.f_str());
            }
            
            delay(5000);
        }

        if (doc["httpCode"].as<int>() == 200)
            return true;
    }

    return false;
}