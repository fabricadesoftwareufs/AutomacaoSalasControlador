#ifndef Controller_h
#define Controller_h
#include "Config.h"

class Controller
{   
    public:
        Controller();
        void restart() const;

        // HTTP
        bool start(struct HardwareRecord&) const;
        bool registerHardware(struct HardwareRecord) const;
        void getSensors(struct HardwareRecord, String *, int&);

        //void getDevices(String[]&);
        bool getMaster(struct HardwareRecord, String &);
        void fillHardwares(struct HardwareRecord);

        bool loadedDevices();

        // BLE
        void setupBLEClient(String deviceName, DeviceType deviceType);   
        void setupBLEServer();   
        void startBLETaskServer();    
        bool notificateServer() const;       

        //send Data Monitoring
        void sendDataOfMonitoring(struct MonitoringRecord);

        // WEB SOCKET
        void startTaskHttp();

        HardwareRecord getHardwareConfig();
        void setHardwareConfig(HardwareRecord hardware);

        // Validacao continua de variaveis de ambiente
        void environmentVariablesContinuousValidation();
        void setupEnvironmentVariables();

        // Send Commands
        void ExecuteCommand(String command);
};

#endif
