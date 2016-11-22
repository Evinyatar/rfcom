#include <Arduino.h>

struct data_dio {
    uint8_t interruptor;
    unsigned long sender;
};

void send_dio(int address, char * arguments);
bool write_config_dio(int address, char * arguments);
char * read_config_dio(int address);