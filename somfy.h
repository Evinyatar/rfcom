#include <Arduino.h>

struct data_somfy {
    uint32_t address;
    uint16_t rollingCode;
};

void send_somfy(int address, char * arguments);
bool write_config_somfy(int address, char * arguments);
char * read_config_somfy(int address);
