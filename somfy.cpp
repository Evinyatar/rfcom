/*   This sketch allows you to emulate a Somfy RTS or Simu HZ remote.
   If you want to learn more about the Somfy RTS protocol, check out https://pushstack.wordpress.com/somfy-rts-protocol/

   The rolling code will be stored in EEPROM, so that you can power the Arduino off.

   Easiest way to make it work for you:
    - Choose a remote number
    - Choose a starting point for the rolling code. Any unsigned int works, 1 is a good start
    - Upload the sketch
    - Long-press the program button of YOUR ACTUAL REMOTE until your blind goes up and down slightly
    - send 'p' to the serial terminal
  To make a group command, just repeat the last two steps with another blind (one by one)

  Then:
    - m, u or h will make it to go up
    - s make it stop
    - b, or d will make it to go down
    - you can also send a HEX number directly for any weird command you (0x9 for the sun and wind detector for instance)
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "common.h"
#include "somfy.h"
#define PORT_TX PIN_T43342 //5 of PORTD = DigitalPin 5

#define SYMBOL 640
#define HAUT 0x2
#define STOP 0x1
#define BAS 0x4
#define PROG 0x8

byte frame[7];
byte checksum;

void BuildFrame(byte *frame, data_somfy *data, byte button) {
    unsigned short code = data->rollingCode;
    frame[0] = 0xA7; // Encryption key. Doesn't matter much
    frame[1] = button << 4;  // Which button did  you press? The 4 LSB will be the checksum
    frame[2] = code >> 8;    // Rolling code (big endian)
    frame[3] = code;         // Rolling code
    frame[4] = data->address >> 16; // Remote address
    frame[5] = data->address >>  8; // Remote address
    frame[6] = data->address;       // Remote address

    /*
    Serial.print("Frame         : ");
    for(byte i = 0; i < 7; i++) {
        if(frame[i] >> 4 == 0) { //  Displays leading zero in case the most significant
            Serial.print("0");     // nibble is a 0.
        }
        Serial.print(frame[i],HEX); Serial.print(" ");
    }
    */

// Checksum calculation: a XOR of all the nibbles
    checksum = 0;
    for(byte i = 0; i < 7; i++) {
        checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
    }
    checksum &= 0b1111; // We keep the last 4 bits only


//Checksum integration
    frame[1] |= checksum; //  If a XOR of all the nibbles is equal to 0, the blinds will
    // consider the checksum ok.
    /*
    Serial.println(""); Serial.print("With checksum : ");
    for(byte i = 0; i < 7; i++) {
        if(frame[i] >> 4 == 0) {
            Serial.print("0");
        }
        Serial.print(frame[i],HEX); Serial.print(" ");
    }
    */

    // Obfuscation: a XOR of all the bytes
    for(byte i = 1; i < 7; i++) {
        frame[i] ^= frame[i-1];
    }

    /*
    Serial.println(""); Serial.print("Obfuscated    : ");
    for(byte i = 0; i < 7; i++) {
        if(frame[i] >> 4 == 0) {
            Serial.print("0");
        }
        Serial.print(frame[i],HEX); Serial.print(" ");
    }
    debug("Rolling Code  : " + String(code));
    */
    
    data->rollingCode = code++; //  We store the value of the rolling code in the
    // EEPROM. It should take up to 2 adresses but the
    // Arduino function takes care of it.
}

void SendCommand(byte *frame, byte sync) {
    if(sync == 2) { // Only with the first frame.
        //Wake-up pulse & Silence
//    PORTD |= 1<<PORT_TX;
        digitalWrite(PORT_TX, HIGH);
        delayMicroseconds(9415);
        digitalWrite(PORT_TX, LOW);
        delayMicroseconds(89565);
    }

    // Hardware sync: two sync for the first frame, seven for the following ones.
    for (int i = 0; i < sync; i++) {
        digitalWrite(PORT_TX, HIGH);
        delayMicroseconds(4*SYMBOL);
        digitalWrite(PORT_TX, LOW);
        delayMicroseconds(4*SYMBOL);
    }

    // Software sync
    digitalWrite(PORT_TX, HIGH);
    delayMicroseconds(4550);
    digitalWrite(PORT_TX, LOW);
    delayMicroseconds(SYMBOL);


    //Data: bits are sent one by one, starting with the MSB.
    for(byte i = 0; i < 56; i++) {
        if(((frame[i/8] >> (7 - (i%8))) & 1) == 1) {
            digitalWrite(PORT_TX, LOW);
            delayMicroseconds(SYMBOL);

            digitalWrite(PORT_TX, HIGH);
            delayMicroseconds(SYMBOL);
        }
        else {
            digitalWrite(PORT_TX, HIGH);
            delayMicroseconds(SYMBOL);

            digitalWrite(PORT_TX, LOW);
            delayMicroseconds(SYMBOL);
        }
    }

    digitalWrite(PORT_TX, LOW);
    delayMicroseconds(30415); // Inter-frame silence
}

bool send_somfy(int address, char* arguments) {
    data_somfy data;
    EEPROM.get(address, data);

    char command;
    if(strcmp(arguments, "up") == 0) command = HAUT;
    else if(strcmp(arguments, "down") == 0) command = BAS;
    else if(strcmp(arguments, "prog") == 0) command = PROG;
    else if(strcmp(arguments, "stop") == 0 || strcmp(arguments, "my") == 0) command = STOP;
    else command = atoi(arguments);

    BuildFrame(frame, &data, command);

    SendCommand(frame, 2);
    for(int i = 0; i<2; i++) {
        SendCommand(frame, 7);
    }

    // store updated rolling code into EEPROM
    EEPROM.update(address + offsetof(struct data_somfy, rollingCode), data.rollingCode + 1);
    return true;
}

bool write_config_somfy(int address, char* arguments) {
    data_somfy newData;
    char* addressStr = strtok(arguments, ",");
    Serial.println(addressStr);
    newData.address = atol(addressStr);
    Serial.println(newData.address);
    newData.rollingCode = atoi(strtok(NULL, ","));
    if(strtok(NULL, ",") != NULL) {
        error("Expected <address>,<rollingCode>");
        return false;
    }

    EEPROM.put(address, newData);
    return true;
}

int read_config_somfy(int address, char* buffer) {
    data_somfy data;
    EEPROM.get(address, data);
    return sprintf(buffer, "%ld,%d", data.address, data.rollingCode);
}
