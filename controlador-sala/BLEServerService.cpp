#include "BLEServerService.h"
#include "Global.h"

int BLEServerService::__countTypeSensor = 0;
int BLEServerService::__countTypeActuator = 0;
std::vector<String> BLEServerService::__sensors;
std::vector<struct HardwareRecord> BLEServerService::__actuators;
BLEScan* BLEServerService::__pBLEScan;
vector<BLEAdvertisedDevice*> BLEServerService::__filteredDevices;
unordered_map<string, Hardware> BLEServerService::__devicesMapped;
BLEDeviceConnect* BLEServerService::__actuatorConnected;
EnvironmentVariablesService __environmentVariables;
AwaitHttpService __clientAwaitHttpService;
Config __configuration;
WiFiService __wfService;

bool IN_CLASS = false; 

bool HTTP_RECEIVED_DATA = false;
String HTTP_MESSAGE = "";

bool ENV_RECEIVED_DATA = false;
String ENV_MESSAGE = "";

bool HTTP_REQUEST = false;
bool ENV_REQUEST = false;

BLEServerService::BLEServerService()
{
    __countTypeSensor = 0;
    __countTypeActuator = 0;
}

void BLEServerService::notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) 
{
    if (isNotify) 
    {
      Serial.println("====================================================");
      Serial.print("[BLEServerService] Notify callback for characteristic: ");
      Serial.println(pBLERemoteCharacteristic->getUUID().toString().c_str());
      Serial.print("[BLEServerService] of data length ");
      Serial.println(length);
      Serial.print("[BLEServerService] data: ");
      String data = String(((char*)pData));
      Serial.println(data.substring(0, length));

      if(HTTP_REQUEST)
      {         
        HTTP_RECEIVED_DATA = true;
        HTTP_MESSAGE = data.substring(0, length);
      }
      else
      {
        ENV_RECEIVED_DATA = true;
        ENV_MESSAGE = data.substring(0, length);  
      }

    }
}
  
BLEDeviceConnect* BLEServerService::connectToDevice(BLEAdvertisedDevice* myDevice, bool validateConnection) 
{
    BLEDeviceConnect* device = new BLEDeviceConnect();
    
    device->deviceFound = true;

    Serial.println("[CONNECTION]: - Create client");
    device->pClient = BLEDevice::createClient();
      
    bool ok = device->pClient->connect(myDevice);
  
    if(!ok) 
    {
      device->deviceFound = false;
      return device;
    } 
    
    delay(200);

    Serial.println("[CONNECTION]: - Connected to disp");
    device->pRemoteService = device->pClient->getService(SERVICE_UUID);
        
    if (device->pRemoteService == nullptr) 
    {
      Serial.print("[CONNECTION]: Failed to find our service UUID: ");
      Serial.println(SERVICE_UUID.toString().c_str());
          
      device->pClient->disconnect();
      device->deviceFound = false;
          
      return device;
    }
    
    delay(200);

    Serial.println("[CONNECTION]: - Found our service");
    device->pRemoteCharacteristic = device->pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
        
    if (device->pRemoteCharacteristic == nullptr) 
    {
      Serial.print("[CONNECTION]: Failed to find our characteristic UUID: ");
      Serial.println(CHARACTERISTIC_UUID.toString().c_str());
          
      device->pClient->disconnect();
      device->deviceFound = false;
          
      return device;
    }
      
    Serial.println("[CONNECTION]: - Found our characteristic");
      
    if (validateConnection)
    {     
      Serial.println("[CONNECTION]: - Valid connection");  
      delay(500);
      if(device->pRemoteCharacteristic->canRead())
      {
        Serial.println("[CONNECTION]: - Characteristic read");  
        delay(500);
        std::string value = device->pRemoteCharacteristic->readValue();

        Serial.print("[CONNECTION]: uuid pesquisado: ");
        Serial.println(value.c_str());
      
        if(!isAtuador(value.c_str()) && !isSensor(value.c_str()))
        {
          Serial.println("[CONNECTION]: device not found ");
          device->pClient->disconnect();
          device->deviceFound = false;
                     
          return device;
        }
      
        device->uuid = value.c_str();
        Serial.print("[CONNECTION]: uuid the device: ");
        Serial.println(device->uuid.c_str());
      }
      else
      {
        Serial.println("[CONNECTION]: device not found ");      
        device->pClient->disconnect();
        device->deviceFound = false;
                          
        return device;
      }  
    } 
      
    if(device->pRemoteCharacteristic->canNotify())
      device->pRemoteCharacteristic->registerForNotify(notifyCallback);
        
    Serial.println("[CONNECTION]: dispositivo conectado ");
    Serial.print("[CONNECTION]: ");
    Serial.println(CHARACTERISTIC_UUID.toString().c_str());
    Serial.print("[CONNECTION]: ");
    Serial.println(myDevice->getAddress().toString().c_str());
  
    Serial.print("[CONNECTION]: connId ");
    Serial.println(device->pClient->getConnId());
    Serial.print("[CONNECTION]: RSSI");
    Serial.println(device->pClient->getRssi());
  
    return device;
}
  
void BLEServerService::initBLE() 
{
    Serial.println("================================================");
    Serial.println("[BLEServerService] Iniciando configuracoes BLE");
    BLEDevice::init("ESP32_CONTROLLER");    
    Serial.println("[BLEServerService] Init device");
}

void BLEServerService::activeBLEScan() 
{
    __pBLEScan = BLEDevice::getScan();
    Serial.println("[BLEServerService] new Scan");
    __pBLEScan->setInterval(1349);
    Serial.println("[BLEServerService] Set interval");
    __pBLEScan->setWindow(449);
    Serial.println("[BLEServerService] Set window");
    __pBLEScan->setActiveScan(true);
    Serial.println("[BLEServerService] Active scan");
}
  
void BLEServerService::scanDevices() 
{
    BLEScanResults foundDevices = __pBLEScan->start(5, false);

    for (int i = 0; i < foundDevices.getCount(); i++)
      __filteredDevices.push_back(new BLEAdvertisedDevice(foundDevices.getDevice(i)));
  
    BLEAdvertisedDevice* disp;
    for (disp : __filteredDevices)
      Serial.println(disp->toString().c_str());    
}

void BLEServerService::stopScan() 
{
    __pBLEScan->setActiveScan(false);
    __pBLEScan->stop();
}
  
/*
     Após fazer a busca, ele faz uma filtragem de quais dispositivos devem ser
*/
void BLEServerService::populateMap() 
{
   for (auto disp : __filteredDevices) 
   {
        if (__configuration.isDebug())
        {
          Serial.println("=============================================");
          Serial.print("[CONNECTION]: Have UUID: ");
          Serial.println(disp->haveServiceUUID());
          Serial.print("[CONNECTION]: Dispositive: ");
          Serial.println(disp->toString().c_str());
        }
        
        if(disp->haveServiceUUID() && disp->isAdvertisingService(SERVICE_UUID))
        {
          bool deviceConnected = false;
          int index = 0, MAX = 4;
          
          do
          {
            if (__configuration.isDebug())
            {
              Serial.println("====================================");
              Serial.print("[CONNECTION]: Attempt Device: ");
              Serial.println(disp->toString().c_str());
              Serial.print("[CONNECTION]: Attempt Number: ");
              Serial.println(index);
              Serial.println("====================================");
            }
            
            if(connectMyDisp(disp))
              break;

            index++;  
          } while(index < MAX);

          Serial.println("[CONNECTION]: Device Found");
        }
        else
        {
           Serial.println("[CONNECTION]: Device no listed");
        }
    }
  
    if (__devicesMapped.size() == 0)
      Serial.println("[CONNECTION]: no devices found");    
}
  
bool BLEServerService::isSensor(String uuid)
{ 
    if(std::count(__sensors.begin(), __sensors.end(), uuid))
      return true;
  
    return false;
}
  
bool BLEServerService::isAtuador(String uuid)
{ 
   for (auto item : __actuators) 
   {
        if(item.uuid.equals(uuid))
          return true;
   }
  
   return false;
}

bool BLEServerService::isSensorListed(String uuid, int typeDisp)
{ 
  Hardware disp;

  for (auto item : __devicesMapped) 
  {
      disp = item.second;

      Serial.println("==================================");         
      Serial.println("[BLEServerService]: mapping uuid: " + disp.getUuid());

      if (uuid.equals(disp.getUuid().c_str()) && disp.getTypeDisp() == typeDisp) 
      {
        Serial.println("[BLEServerService]: device found");
        return true;
      }
  }

  return false;
}
  
bool BLEServerService::connectMyDisp(BLEAdvertisedDevice* device) 
{
    Serial.println();
    Serial.println("[CONNECTION]: connecting to my devices...");
  
    BLEDeviceConnect *deviceConnected = connectToDevice(device, true);
  
    if (deviceConnected->deviceFound)
    {
      Serial.println("[CONNECTION]: forming successfull a connection");
  
      Hardware disp;
      disp.setBLEAdvertisedDevice(device);
      disp.setUuid(deviceConnected->uuid.c_str());

      if(isSensor(deviceConnected->uuid.c_str()))
      {
        Serial.println("[CONNECTION]: Is Sensor ");
        disp.setTypeDisp(TYPE_SENSOR);
        __countTypeSensor++;
      }
      else if(isAtuador(deviceConnected->uuid.c_str()))
      {
        Serial.println("[CONNECTION]: Is Actuator ");
        disp.setTypeDisp(TYPE_ACTUATOR);
        __countTypeActuator++;
      }
  
      __devicesMapped.insert(std::make_pair(deviceConnected->uuid.c_str(), disp));
      Serial.println("[CONNECTION]: connection saved. finalized");
  
      if(deviceConnected->pClient->isConnected())    
        deviceConnected->pClient->disconnect(); 
        
      free(deviceConnected->pClient);
      free(deviceConnected->pRemoteCharacteristic);
      free(deviceConnected->pRemoteService);
      delete deviceConnected;
      
      return true;
    }
    else
    {
      Serial.println("[CONNECTION]: have problems in connection");
      
      free(deviceConnected->pClient);
      free(deviceConnected->pRemoteCharacteristic);
      free(deviceConnected->pRemoteService);
      delete deviceConnected;
      
      return false;
    }
}
  
void BLEServerService::sendMessageToActuator(String data) 
{        
    if(__actuatorConnected->pClient->isConnected())
        __actuatorConnected->pRemoteCharacteristic->writeValue(std::string(data.c_str()), false);
}

void BLEServerService::disconnectToActuator() 
{    
    delay(1000);
    
    if(__actuatorConnected->pClient->isConnected())
      __actuatorConnected->pClient->disconnect();
    
    delete __actuatorConnected;
}

bool BLEServerService::connectToActuator(String uuidDevice) 
{        
    Hardware disp;
    bool connected = false;

    Serial.println("==========================================");
    for (auto item : __devicesMapped) 
    {
      disp = item.second;
      
      Serial.println("[BLEServerService]: uuid mapped: " + disp.getUuid());
      if (uuidDevice.equals(disp.getUuid().c_str())) 
      {
        Serial.println("[BLEServerService]: device found");
        __actuatorConnected = connectToDevice(disp.getBLEAdvertisedDevice(), false);

        Serial.println("[BLEServerService]: device found: ");
        Serial.println(__actuatorConnected->pClient->isConnected());
        if(__actuatorConnected->pClient->isConnected())
        {
          connected = true;
          break;
        }
      }
    }

    return connected;
}
  
  
void BLEServerService::continuousConnectionTask() 
{  
  bool longTimeWithoutConnections = false;

  while (true)
  {
    vTaskDelay(pdMS_TO_TICKS(TIME_WAITING_CONNECTION));

    Serial.println("=========================================================");
    Serial.println("[CONTINUOUS_CONNECTION] Actual Time: " + String(millis()));

    if(!HTTP_REQUEST && !ENV_REQUEST)
    {
      __configuration.lockEnvVariablesMutex();

      __wfService.disconnect();

      newCicle();  

      __wfService.connect();

      __configuration.unlockEnvVariablesMutex();
    }
  }
}

void BLEServerService::newCicle()
{
    BLEDeviceConnect *deviceConnected;
    vector<BLEDeviceConnect*> aux;
    bool isDeviceConected = false;
    Hardware disp;
    int count = 0;

    Serial.println("=================================");
    Serial.println("[CONTINUOUS_CONNECTION] NEW CICLE");
        
    for (auto item : __devicesMapped) 
    {
      if (__configuration.isDebug())
      {
            Serial.println("=================================");
            Serial.println("[CONTINUOUS_CONNECTION] Receive Request: " + String(HTTP_REQUEST));
            Serial.println("[CONTINUOUS_CONNECTION] Env Request: " + String(ENV_REQUEST));
            Serial.println("[CONTINUOUS_CONNECTION] In Class: " + String(IN_CLASS));
      }
           
      if(HTTP_REQUEST || ENV_REQUEST)
      {
        if(!aux.empty()) 
        {
          closeConnections(aux);
          aux.clear();
        }

        Serial.println("[CONTINUOUS_CONNECTION] Request Enabled or No Class");

        return;
      }
            
      disp = item.second;
                
      if(disp.getTypeDisp() == TYPE_SENSOR)
      {
        if (__configuration.isDebug())
        { 
          Serial.println("================================");
          Serial.println("[CONTINUOUS_CONNECTION] UUID: " + String(disp.getUuid()));
          Serial.println("[CONTINUOUS_CONNECTION] ADDRESS: " + String(disp.getBLEAdvertisedDevice()->getAddress().toString().c_str()));
        }
                    
        deviceConnected = connectToDevice(disp.getBLEAdvertisedDevice(), false);
              
        if(deviceConnected->pClient->isConnected())
        {
          String data = "GET_DATA";
          deviceConnected->pRemoteCharacteristic->writeValue(data.c_str(), data.length());
          isDeviceConected = true;
        }
              
        aux.push_back(deviceConnected);
              
        count++;
                  
        if ((count % 3) == 0 || __countTypeSensor == count)
        {
          if(isDeviceConected)
            timer();
                                                
            closeConnections(aux);
            aux.clear();
        }
      } 
    }
}


void BLEServerService::closeConnections(vector<BLEDeviceConnect*> aux)
{
  for (auto deviceCon : aux) 
  {
    Serial.println();
    Serial.print("[DISCONNECT]: ");
    Serial.println(deviceCon->pClient->getPeerAddress().toString().c_str());
                            
    delay(300);
    if(deviceCon->pClient->isConnected())
      deviceCon->pClient->disconnect();
                              
    delay(300);  
    free(deviceCon->pClient);
    free(deviceCon->pRemoteCharacteristic);
    free(deviceCon->pRemoteService);
                            
    delete deviceCon;
  }
}

vector<String> BLEServerService::getSensors()
{
  return __sensors;
}

vector<struct HardwareRecord> BLEServerService::getActuators()
{
  return __actuators;
}

void BLEServerService::setCountTypeSensor(int count)
{
  __countTypeSensor = count;
}

void BLEServerService::setCounttypeActuator(int count)
{
  __countTypeActuator = count;
}

int BLEServerService::getCountTypeSensor()
{
  return __countTypeSensor;
}

int BLEServerService::getCounttypeActuator()
{
  return __countTypeActuator;
}

void BLEServerService::addSensor(String sensor)
{
  __sensors.push_back(sensor);
}

void BLEServerService::addActuator(HardwareRecord act)
{
  __actuators.push_back(act);
}
  
void BLEServerService::timer()
{     
   unsigned long tempoLimite = millis() + TIME_CONNECTION;
    
   while(millis() <= tempoLimite && !HTTP_REQUEST && !ENV_REQUEST){
     
     if((millis() % 5000) == 0)
     {
       Serial.println();
       Serial.print("[MILLIS]: MARCAÇÃO TEMPORAL: ");
       Serial.println();
       Serial.print("[MILLIS]: tempo atual: ");
       Serial.println(millis());
       Serial.print("[MILLIS]: tempo limite: ");
       Serial.println(tempoLimite);
       Serial.println();
     }
   };
}

void BLEServerService::startTaskBLEImpl(void* _this)
{
    BLEServerService* bleSettings = (BLEServerService*)_this;
    bleSettings->continuousConnectionTask();
}

void BLEServerService::startTaskBLE()
{
    xTaskCreate(this->startTaskBLEImpl, "Task", 8192, this, 8, NULL);
}