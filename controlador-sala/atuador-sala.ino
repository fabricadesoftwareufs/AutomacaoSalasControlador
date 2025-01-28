/*#include "Global.h"
#include "Config.h"

#define RELE 19

HardwareRecord hardware;
WiFiService wiFiService;
Controller controller;
String master = "";

bool SEND_DATA = false;
String COMMAND;
const uint16_t kIrLed = 12;
IRsend irsend(kIrLed); 

void setup() {

	Serial.begin(115200);
  pinMode(RELE, OUTPUT);
	irsend.begin();
	bool init = false;

	wiFiService.connect();

	do {
		if ( controller.start(hardware) ) {
			if ( controller.registerHardware(hardware) ) {
				if(controller.getMaster(hardware, master))
				{
					Serial.print("master: ");
					Serial.println(master);
					init = true;
				}
				controller.setHardwareConfig(hardware);

			}
		}
	} while( !init ); 

	wiFiService.disconnect();

    controller.setupBLEClient("ESP_ATUADOR", ATUADOR);  
}

void loop() {
  
  Serial.println("[Loop] Await message"); 
  
  if(SEND_DATA) {
    Serial.println("[Loop] FOWARD TO SEND COMMAND"); 
    controller.ExecuteCommand(COMMAND);
    SEND_DATA = false;
  }

  delay(200);
} */