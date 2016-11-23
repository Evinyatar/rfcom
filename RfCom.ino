#include <Arduino.h>
#include <EEPROM.h>
#include "common.h"
#include "configstore.h"

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

        if(strcmp(argv[0], "help") == 0) {
            log("Commands:");
            log(" initialize");
            log("    Erase settings.");
            log(" add_device <id> <protocol_id> <configuration>");
            log("    Add a device");
            log(" list_devices");
            log("    List all devices and their configuration");
            log(" config <id> <configuration>");
            log("    Change existing device configuration");
            log(" send <id> <arguments>");
            log("    Send command");
        } else if(strcmp(argv[0], "add_device") == 0) {
            if(argc != 4) {
                error("Usage: add_device <id> <protocol_id> <arguments>");
                return;
            }
            int id = atoi(argv[1]);
            ProtocolId protocolId = (ProtocolId) atoi(argv[2]);
            if(id < 0 || id > 255) {
                error("id should be between 0 and 255");
                return;
            }

            if(protocolId >= MAX_PROTOCOL || protocolId == UNKNOWN_PROTOCOL) {
                error("protocol " + String(protocolId) + " not recognized. Try list_protocols.");
                return;
            }

            if(findDeviceForId(id) != NULL) {
                error("Id " + String(id) + " already in use.");
                return;
            }

            if(addDevice(id, protocolId, argv[3])) {
                Serial.println("Success");
            } else {
                Serial.println("Failed");
            }

        } else if(strcmp(argv[0], "list_devices") == 0) {
            Serial.println("Listing devices:");
            DeviceSearch context;
            if(!startSearch(&context)) {
                return;
            }
            Serial.print("Number of devices: "); Serial.println(context.deviceCount);
            while(nextDevice(&context)) {
                Serial.println("Index: " + String(context.index));
                Serial.println("EEPROM Offset: " + String(context.device.address));
                Serial.println("Device Id: " + String(context.device.id));
                Serial.println("Protocol: " + String(context.device.protocolId) + " - " + context.device.protoSpec->description);
                Serial.print("Configuration: ");
                Serial.println(context.device.protoSpec->readConfig(context.device.address));
                Serial.println("================================================================");
            }
        } else if(strcmp(argv[0], "initialize") == 0) {
            Serial.println("Erasing");
            initialize();
        } else if(strcmp(argv[0], "config") == 0) {
            if(argc != 3) {
                error("Usage: config <id> <configuration>");
                return;
            }
            
            int id = atoi(argv[1]);
            ProtocolId protocolId = (ProtocolId) atoi(argv[2]);
            if(id < 0 || id > 255) {
                error("id should be between 0 and 255");
                return;
            }

            Device* device = findDeviceForId(id);
            if(device == NULL) {
                error("Id could not be found");
                return;
            }

            device->protoSpec->writeConfig(device->address, argv[2]);
        } else if(strcmp(argv[0], "send") == 0) {
            if(argc != 3) {
                error("Usage: send <id> <arguments>");
                return;
            }
            
            int id = atoi(argv[1]);
            ProtocolId protocolId = (ProtocolId) atoi(argv[2]);
            if(id < 0 || id > 255) {
                error("id should be between 0 and 255");
                return;
            }
            
            Device* device = findDeviceForId(id);
            if(device == NULL) {
                error("Id could not be found");
                return;
            }

            device->protoSpec->send(device->address, argv[2]);
        } else {
            Serial.print("Unknown command ");
            Serial.println(argv[0]);
        }
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
