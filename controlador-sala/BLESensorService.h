#ifndef BLESensorService_h
#define BLESensorService_h

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <String>
#include "Global.h"
#include "EquipmentService.h"

#define CHARACTERISTICUUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVICEUUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define GET_DATA  "GET_DATA"
#define END_DATA  "END_DATA"
#define LED 2

static BLECharacteristic* pCharacteristicSensor;  
static bool deviceConnected;
static BLEServer* pServer;
EnvironmentVariablesService __environmentVariableService;
static DeviceType deviceType;
EquipmentService equipmentService;
static String equipmentState = "";
static String receivedData = "";

void sendDataToServer(String data)
{
  if (deviceConnected) 
  {
      if(SEND_DATA)
      {
        pCharacteristicSensor->setValue(data.c_str());
        pCharacteristicSensor->notify();
        delay(100); 
      }
  }
}

class MyServerCallbacks: 
    public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {

      digitalWrite(LED, HIGH);
      pCharacteristicSensor->setValue(__environmentVariableService.getHardware().uuid.c_str());
      pCharacteristicSensor->notify();
      
      deviceConnected = true;
      SEND_DATA = false;

      Serial.println("===============================================");
      Serial.println("[BLESensorService] CONECTADO");
            
      delay(100);
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;      
      digitalWrite(LED, LOW);
      Serial.println("===============================================");
      Serial.println("[BLESensorService] DESCONECTADO");

      ESP.restart();
    }
};


class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic * pCharacteristic) {

    std::string response = pCharacteristic->getValue();
    Serial.println("===============================================");
    Serial.println("[BLESensorService] Receive packet: " + String(response.c_str()));

    if(String(GET_DATA).equals(response.c_str())) 
    {
      SEND_DATA = true;
    } 
    else if(deviceType != NULL && deviceType == ATUADOR)
    {
      if(String(END_DATA).equals(response.c_str()))
      {
        Serial.println("===============================================");
        Serial.println("[BLESensorService] ATUADOR - (ONWRITE) COMMANDO PARA O EQUIPAMENTO");
        SEND_DATA = true;
        COMMAND = receivedData;
        
        equipmentState = "";
        receivedData = "";
      }
      else
      {
        receivedData = receivedData + response.c_str();
      }
    } 
  }
};

void initBLEClient(String deviceName, DeviceType devType)
{
  pinMode(LED, OUTPUT);
  deviceType = devType;
  
  Serial.println("========================================");
  Serial.println("[BLE_CLIENT]: Set Name Disp");
  BLEDevice::init(std::string(deviceName.c_str()));

  Serial.println("[BLE_CLIENT]: Create server");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  Serial.println("[BLE_CLIENT]: Create Service");
  BLEService *pService = pServer->createService(SERVICEUUID);

  Serial.println("[BLE_CLIENT]: Create Characteristic");
  pCharacteristicSensor = pService->createCharacteristic(
                      CHARACTERISTICUUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristicSensor->addDescriptor(new BLE2902());

  pCharacteristicSensor->setCallbacks(new MyCallbacks());

  Serial.println("[BLE_CLIENT]: Start Service");
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICEUUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();

  Serial.println("Waiting a client connection to notify...");
}
    
#endif
