#include "protocols.h"

#define EEPROM_START 0
#define EEPROM_VERSION 10

struct DeviceSearch {
    int nextOffset; // address of the header of the next device
    byte deviceCount; // number of configured devices in the config
    int index; // index of the current device in the configuration
    Device device; // current device
};

bool startSearch(DeviceSearch * context);
bool nextDevice(DeviceSearch * context);
Device* findDeviceForId(short idToFind);
bool addDevice(byte id, ProtocolId protocolId, char * arguments);
void initialize();
