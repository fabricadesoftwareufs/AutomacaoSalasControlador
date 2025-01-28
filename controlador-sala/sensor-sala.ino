/*#include "Config.h"

HardwareRecord hardwareSensor;
MonitoringRecord monitoringRecord;
WiFiService wiFiService;
Controller controller;
String master = "";
String COMMAND;
bool SEND_DATA = false;
Config conf;

DHT dht(4, DHT11);
float temperature; 
const int portaPresenca = GPIO_NUM_12;

int qtdDetectouPresenca = 0;

void setup() {
	
  Serial.begin(115200);
  bool init = false;

  wiFiService.connect();

  do {
    if ( controller.start(hardwareSensor) ) {
      if ( controller.registerHardware(hardwareSensor) ) {
        if(controller.getMaster(hardwareSensor, master))
        {
          Serial.print("master: ");
          Serial.println(master);
          init = true;
        }
        
        controller.setHardwareConfig(hardwareSensor);
      }
    }
  } while( !init );
  
  wiFiService.disconnect();

  pinMode(portaPresenca, INPUT);

  controller.setupBLEClient("ESP_SENSOR_" + hardwareSensor.id, SENSOR);
}

void loop() {
  Serial.println("[INO]: loop");
    
  bool leitura = digitalRead(portaPresenca);
  temperature = dht.readTemperature();

  if(leitura) {
    qtdDetectouPresenca++;
  }

  if(SEND_DATA) {
		
    Serial.println("[INO]: data solicited ");

    monitoringRecord.hasPresent = qtdDetectouPresenca >= conf.getTimesToHasOne() ? "S" : "N";
        
    monitoringRecord.temperature = temperature;
    
    controller.sendDataOfMonitoring(monitoringRecord);
        
    SEND_DATA = false;
        
    qtdDetectouPresenca = 0;
  }

  delay(1000);
}*/