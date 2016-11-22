#include <Arduino.h>
#include <EEPROM.h>
#include "common.h"
#include "protocols.h"

#define EEPROM_START 0
#define EEPROM_VERSION 10

void setup() {
    pinMode(PIN_T43342, OUTPUT);
    pinMode(PIN_T43392, OUTPUT);
    Serial.begin(115200);

    if(EEPROM_VERSION != EEPROM.read(EEPROM_START)) {
        log("EEPROM version number mismatch.");
    }

    Serial.println("RfCom 0.1");
}

char* argv[10];

int findOffsetAfterLastDevice() ;

int findDeviceForId(short idToFind) ;

typedef bool (*iterateFunc)(int index, int address, byte id, Protocol *protoSpec);

int iterateDevices(iterateFunc func) ;

void loop() {
    if(Serial.available() > 0) {
        char chars[100];
        char* buffer = (char *) &chars;
        int count = Serial.readBytesUntil('\n', buffer, sizeof(chars));

        buffer[count] = 0;
        
        int argc = 0;
        int idx = 0;
        int oidx = 0;
        char* argv[10];

        argv[argc++] = strtok(buffer, " ");
        while((argv[argc] = strtok(NULL, " ")) != NULL) {
            argc++;
        }

        if(strcmp(argv[0], "add_device") == 0) {
            if(argc != 4) {
                error("Usage: add_device <id> <protocol_id> <arguments>");
            }
            int id = atoi(argv[1]);
            ProtocolId protocolId = (ProtocolId) atoi(argv[2]);
            if(id < 0 || id > 255) {
                error("id should be between 0 and 255");
                return;
            }

            if(protocolId >= MAX_PROTOCOL || protocolId == UNKNOWN_PROTOCOL) {
                log(sprintf("protocol %d not recognized. Try list_protocols.", (int) protocolId));
                return;
            }

            if(addDevice(id, protocolId, argv[3])) {
                Serial.println("Success");
            } else {
                Serial.println("Failed");
            }
            
        } else if(strcmp(argv[0], "list_devices") == 0) {
            Serial.println("Listing devices:");
            iterateDevices(printDevice);
        } else if(strcmp(argv[0], "initialize") == 0) {
            Serial.println("Erasing");
            initialize();          
        } else {
            Serial.print("Unknown command ");
            Serial.println(argv[0]);
        }
    }
}

void initialize() {
    EEPROM.write(EEPROM_START, EEPROM_VERSION);
    EEPROM.write(EEPROM_START + 1, 0);
}

bool printDevice(int index, int address, byte id, Protocol* protoSpec) {
    Serial.println("Index: " + String(index));
    Serial.println("EEPROM Offset: " + String(address));
    Serial.println("Device Id: " + String(id));
    Serial.println("Protocol: " + String(protoSpec->protocol) + " - " + protoSpec->description);
    Serial.println("Configuration: " + protoSpec->readConfig(address));
    Serial.println("================================================================");
    return false;
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

int findOffsetAfterLastDevice() {
    return findDeviceForId(-1);
}

int findDeviceForId(short idToFind) {
    int offset = EEPROM_START;

    if(EEPROM_VERSION != EEPROM.read(offset++)) {
        Serial.println("EEPROM version number mismatch.");
        return -2;
    }
    byte numberOfDevices = EEPROM.read(offset++);
    while(numberOfDevices > 0) {
        ProtocolId proto = EEPROM.read(offset++);
        byte id = EEPROM.read(offset++);

        if(id != -1 && id == idToFind) {
            return offset;
        }

        Protocol protoSpec = PROTOCOLS[proto];
        offset += protoSpec.dataSize;

        numberOfDevices--;
    }

    return idToFind == -1 ? offset : -1;
}

int iterateDevices(iterateFunc func) {
    int offset = EEPROM_START;

    if(EEPROM_VERSION != EEPROM.read(offset++)) {
        Serial.println("EEPROM version number mismatch.");
        return -2;
    }
    byte numberOfDevices = EEPROM.read(offset++);
    Serial.print("Number of devices: ");
    Serial.println(numberOfDevices);
    int index = 0;
    while(numberOfDevices > index) {
        ProtocolId proto = EEPROM.read(offset++);
        byte id = EEPROM.read(offset++);
        Protocol protoSpec = PROTOCOLS[proto];

        if(func(index, offset, id, &protoSpec)) {
            return offset;
        }

        offset += protoSpec.dataSize;

        index++;
    }
}

void error(String text) {
    Serial.println(text);
}

void output(String text) {
    Serial.println(text);
}

void log(String text) {
    Serial.println(text);
}
