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

typedef void (*Handler)(int address, char* arguments);
typedef bool (*WriteConfig)(int address, char* arguments);
typedef String (*ReadConfig)(int address);

struct Protocol {
    ProtocolId protocol;
    char* description;
    int dataSize;
    Handler handler;
    WriteConfig writeConfig;
    ReadConfig readConfig;
};

Protocol PROTOCOLS[] = {
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

