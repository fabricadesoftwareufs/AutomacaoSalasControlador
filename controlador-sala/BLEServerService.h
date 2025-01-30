#ifndef BLEServerService_h
#define BLEServerService_h

#include <unordered_map>
#include "Hardware.h"
#include <Arduino.h>
#include "BLEScan.h"
#include <BLEDevice.h>
#include "Structs.h"

#define TIME_CONNECTION  5000 
#define TIME_WAITING_CONNECTION 180000

static BLEUUID CHARACTERISTIC_UUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");
static BLEUUID SERVICE_UUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

using namespace std;

class BLEServerService 
{
  public:
    static int __countTypeSensor;
    static int __countTypeActuator;
    static vector<String> __sensors;
    static vector<struct HardwareRecord> __actuators;
    static BLEScan* __pBLEScan;
    static vector<BLEAdvertisedDevice*> __filteredDevices;
    static unordered_map<string, Hardware> __devicesMapped;
    static BLEDeviceConnect* __actuatorConnected;
    static bool __receivedData;
    
    BLEServerService();
    
    static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify); 
    BLEDeviceConnect* connectToDevice(BLEAdvertisedDevice* myDevice, bool validateConnection); 
    void initBLE(); 
    void stopScan(); 
    void scanDevices(); 
    void populateMap(); 
    void activeBLEScan(); 
    bool isSensor(String uuid);
    bool isAtuador(String uuid);
    bool connectMyDisp(BLEAdvertisedDevice* device); 
    int getCountDispsTypeSensor();
    void timer(); 
    bool connectToActuator(String uuidDevice);
    void disconnectToActuator();
    void sendMessageToActuator(String data);

    // getters and setters
    vector<String> getSensors();
    vector<struct HardwareRecord> getActuators();
    void addSensor(String uuid);
    void addActuator(HardwareRecord act);
    void newCicle();
    
    static void setCountTypeSensor(int count);
    static void setCounttypeActuator(int count);
    static int getCountTypeSensor();
    static int getCounttypeActuator();

    void closeConnections(vector<BLEDeviceConnect*> aux);

    bool isSensorListed(String uuid, int typeDisp);

    // metods task
    void continuousConnectionTask();
    static void startTaskBLEImpl(void*);
    void startTaskBLE();
  
};

#endif
