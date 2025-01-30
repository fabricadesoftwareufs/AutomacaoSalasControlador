#include "UtilsService.h"

EnvironmentVariablesService __environmentVariablesService;
HTTPService __httpUtilService;

UtilsService::UtilsService(){}

String UtilsService::mountPayload(String deviceType, String state, String command)
{
    String payload;
    payload.concat("{");
    payload.concat("\"type\":\"" + deviceType + "\", ");
    payload.concat("\"state\":\"" + state + "\", ");
    payload.concat("\"command\":\"" + command + "\"");
    payload.concat("}");

    return payload;
}

void UtilsService::updateMonitoring(String message)
{
    if(message == LZ_ON || message == LZ_OFF)
    {
        Monitoramento monitoringLight = __environmentVariablesService.getMonitoringLight();

        monitoringLight.estado = (message == LZ_ON);

        __environmentVariablesService.setMonitoringLight(monitoringLight);

        if(monitoringLight.equipamentoId > 0 && monitoringLight.id > 0)
           __httpUtilService.putMonitoring(monitoringLight);
    }

    if(message == AC_OFF || message == AC_ON)
    {
       Monitoramento monitoringConditioner = __environmentVariablesService.getMonitoringConditioner();    

       monitoringConditioner.estado = (message == AC_ON);

       __environmentVariablesService.setMonitoringConditioner(monitoringConditioner); 

       if(monitoringConditioner.equipamentoId > 0 && monitoringConditioner.id > 0)
           __httpUtilService.putMonitoring(monitoringConditioner);
    }

}
 
std::vector<String> UtilsService::splitPayload(String payload, int maxSize)
{
    std::vector<String> subStrings;
    String packet = "";
    int index = 0;

    do
    {
        packet = packet + payload[index];

        if ((index != 0 && (index % MAX_LENGTH_PACKET) == 0) || (payload.length() - 1) == index)  
        {
           subStrings.push_back(packet);
           packet = "";
        }

        index++;

    } while(index < payload.length());

    subStrings.push_back(END_DATA);
 
    return subStrings;  
}

String UtilsService::mountDataMonitoring(MonitoringRecord record) 
{
    Serial.println("[UtilsService] mountDataMonitoring");
    DynamicJsonDocument doc(512);
    String data;
    doc["temperature"] = (!isnan(record.temperature) && record.temperature > 0 && record.temperature != NULL) ? record.temperature : -1;
    doc["hasPresent"] = record.hasPresent;
    serializeJson(doc, data);
    return data;
}