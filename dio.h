#include <Arduino.h>

struct data_dio {
    uint8_t interruptor;
    uint32_t sender;
};

bool send_dio(int address, char * arguments);
bool write_config_dio(int address, char * arguments);
int  read_config_dio(int address, char * buffer);
