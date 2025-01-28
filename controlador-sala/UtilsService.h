#ifndef UtilsService_h
#define UtilsService_h

#include "Config.h"

#define AC_ON  "AC-ON"
#define AC_OFF "AC-OFF" 
#define LZ_ON  "LZ-ON" 
#define LZ_OFF "LZ-OFF"
#define END_DATA "END_DATA" 

#define TYPE_CONTROLLER 1
#define TYPE_ACTUATOR 2
#define TYPE_SENSOR 3
#define MAX_LENGTH_PACKET 500

class UtilsService
{
    public:
        UtilsService();
        String mountPayload(String deviceType, String state, String command);
        void updateMonitoring(String message);
        std::vector<String> splitPayload(String payload, int maxSize);
        String mountDataMonitoring(MonitoringRecord record);
};

#endif
