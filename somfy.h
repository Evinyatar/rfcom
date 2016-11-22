#include <Arduino.h>

struct data_somfy {
    unsigned int address;
    unsigned short rollingCode;
};

void send_somfy(int address, char * arguments);
bool write_config_somfy(int address, char * arguments);
char * read_config_somfy(int address);