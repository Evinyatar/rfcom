#include <Arduino.h>
#include "dio.h"
#include "somfy.h"

enum ProtocolId {
    UNKNOWN_PROTOCOL,
    DIO_PROTOCOL,
    SOMFY_PROTOCOL,
    // insert new protocols here
    MAX_PROTOCOL
};

typedef void (*Send)(int address, char* arguments);
typedef bool (*WriteConfig)(int address, char* arguments);
typedef char* (*ReadConfig)(int address);

struct Protocol {
    ProtocolId protocol;
    char* description;
    int dataSize;
    Send send;
    WriteConfig writeConfig;
    ReadConfig readConfig;
};

struct Device {
    int address; // address of the config of the current device
    byte id; // id of the current device
    ProtocolId protocolId; // protocol of the current device
    Protocol *protoSpec; // specification of the current device's protocol
};

const Protocol PROTOCOLS[] = {
        {},
        {
                DIO_PROTOCOL,
                "Chacon Dio",
                sizeof(data_dio),
                send_dio,
                write_config_dio,
                read_config_dio
        },
        {
                SOMFY_PROTOCOL,
                "Somfy",
                sizeof(data_somfy),
                send_somfy,
                write_config_somfy,
                read_config_somfy
        }
};
