#ifndef Hardware_h
#define Hardware_h

#include "Config.h"

class Hardware
{
    private:
        int __id;
        String __uuid;
        String __token;

        int __typeDisp;
        String __characteristic;
        BLEAdvertisedDevice* __bleAdvertisedDevice;

        

    public:       
        Hardware();

        void   restart();
        String getMacAddress() const;

        void   setId(int);
        void   setUuid(String);
        void   setToken(String);
        void   setTypeDisp(int id);
        void   setCharacteristic(String uuid);
        void   setBLEAdvertisedDevice(BLEAdvertisedDevice *bleAdvertisedDevice);
      
        int    getId() const; 
        String getUuid() const; 
        String getToken() const;
        int getTypeDisp();
        String getCharacteristic();
        BLEAddress getMacAddressAdvertisedDevice();
        BLEAdvertisedDevice* getBLEAdvertisedDevice();
        
};

#endif
