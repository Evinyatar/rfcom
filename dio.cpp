#include <Arduino.h>
#include <EEPROM.h>
#include "dio.h"
#include "common.h";

#define TX_PORT PIN_T43392

void sendBit(bool b) {
    if (b) {
        digitalWrite(TX_PORT, HIGH);
        delayMicroseconds(310);   //275 originally, but tweaked.
        digitalWrite(TX_PORT, LOW);
        delayMicroseconds(1340);  //1225 originally, but tweaked.
    } else {
        digitalWrite(TX_PORT, HIGH);
        delayMicroseconds(310);   //275 originally, but tweaked.
        digitalWrite(TX_PORT, LOW);
        delayMicroseconds(310);   //275 originally, but tweaked.
    }
}

void sendPair(bool b) {
    if (b) {
        sendBit(true);
        sendBit(false);
    } else {
        sendBit(false);
        sendBit(true);
    }
}

void transmit(unsigned long sender, uint8_t interruptor, int blnOn) {
    int i;

    digitalWrite(TX_PORT, HIGH);
    delayMicroseconds(275);
    digitalWrite(TX_PORT, LOW);
    delayMicroseconds(9900);   // first lock
    digitalWrite(TX_PORT, HIGH);   // high again
    delayMicroseconds(275);    // wait
    digitalWrite(TX_PORT, LOW);    // second lock
    delayMicroseconds(2675);
    digitalWrite(TX_PORT, HIGH);

    // Emiter ID
    for(i = 0; i < 26; i++) {
        sendPair( (sender >> (25 - i)) & 0b1);
    }

    // 26th bit -- grouped command
    sendPair(false);

    // 27th bit -- On or off
    sendPair(blnOn);

    // 4 last bits -- reactor code 0000 -&gt; 0 -- 0001 -&gt; 1
    for (i = 0; i < 4; i++) {
        sendPair( (interruptor >> (3 - i)) & 1 );
    }

    digitalWrite(TX_PORT, HIGH);   // lock - end of data
    delayMicroseconds(275);    // wait
    digitalWrite(TX_PORT, LOW);    // lock - end of signal
}

bool send_dio(int address, char * arguments) {
    data_dio data;
    EEPROM.get(address, data);
    bool state = strcmp(arguments, "on") == 0;
    
    for (int i = 0; i < 5; i++) {
        transmit(data.sender, data.interruptor, state);
        delay(10);
    }
    return true;
}

bool write_config_dio(int address, char * arguments) {
    data_dio newData;
    char* interruptor = strtok(arguments, ",");
    char* sender = strtok(NULL, ",");
    newData.interruptor = atoi(interruptor);
    newData.sender = atol(sender);
    if(strtok(NULL, ",")) {
        error("Too many arguments: Expected <interruptor>,<sender>");
        return false;
    }

    EEPROM.put(address, newData);

    return true;
}

int read_config_dio(int address, char* buffer) {
    data_dio data;
    EEPROM.get(address, data);
    return sprintf(buffer, "%d,%ld", data.interruptor, data.sender);
}
