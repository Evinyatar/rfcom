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

typedef bool (*Send)(int address, char* arguments);
typedef bool (*WriteConfig)(int address, char* arguments);
typedef int  (*ReadConfig)(int address, char* buffer);

struct Protocol {
    ProtocolId protocol;
    char* description;
    char* configDescription;
    char* commandDescription;
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
                "<interruptor>,<sender>",
                "[on|off]",
                sizeof(data_dio),
                send_dio,
                write_config_dio,
                read_config_dio
        },
        {
                SOMFY_PROTOCOL,
                "Somfy",
                "<address>,<rolling code>",
                "[up|down|stop|my]",
                sizeof(data_somfy),
                send_somfy,
                write_config_somfy,
                read_config_somfy
        }
};
