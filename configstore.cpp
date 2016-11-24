#include <EEPROM.h>
#include "common.h"
#include "configstore.h"

/**
 * Initiate a device search.
 */
bool startSearch(DeviceSearch * context) {
    int offset = EEPROM_START;
    int version = EEPROM.read(offset++);

    if(EEPROM_VERSION != version) {
        error("EEPROM version number mismatch.");
        return false;
    }
    byte numberOfDevices = EEPROM.read(offset++);

    context->index = -1;
    context->device.address = 0;
    context->deviceCount = numberOfDevices;
    context->nextOffset = offset;
    return true;
}

/**
 * Search for the next device. Returns true if successful.
 */
bool nextDevice(DeviceSearch * context) {
    if(context->deviceCount -1 == context->index) {
        return false;
    }
    int offset = context->nextOffset;
    context->index += 1;
    context->device.protocolId = EEPROM.read(offset++);
    context->device.id = EEPROM.read(offset++);
    context->device.address = offset;
    context->device.protoSpec = &PROTOCOLS[context->device.protocolId];
    context->nextOffset = offset + context->device.protoSpec->dataSize;
    return true;
}

void initialize() {
    EEPROM.write(EEPROM_START, EEPROM_VERSION);
    EEPROM.write(EEPROM_START + 1, 0);
}

Device* findDeviceForId(short idToFind) {
    DeviceSearch context;
    if(!startSearch(&context)) {
        return -2;
    }

    while(nextDevice(&context)) {
        if(context.device.id == idToFind) {
            return &context.device;
        }
    }

    return NULL;
}

int findOffsetAfterLastDevice() {
    DeviceSearch context;
    if(!startSearch(&context)) {
        return -2;
    }
    while(nextDevice(&context));
    
    return context.nextOffset;
}

bool addDevice(byte id, ProtocolId protocolId, char * arguments) {
    int lastOffset = findOffsetAfterLastDevice();
    Protocol protoSpec = PROTOCOLS[protocolId];

    bool success = protoSpec.writeConfig(lastOffset + 2, arguments);
    if(success) {
        byte numberOfDevices = EEPROM.read(EEPROM_START + 1);
        EEPROM.write(EEPROM_START + 1, numberOfDevices + 1);
        EEPROM.write(lastOffset, protocolId);
        EEPROM.write(lastOffset + 1, id);
    }
    return success;
}
