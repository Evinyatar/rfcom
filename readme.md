RfCom
=====

General purpose Arduino based 433MHz transmitter. Supports 433.42MHz and 433.92MHz transmitters on pin 5 and pin 6
respectively (configured in "common.h").

To get a 433.42MHz transmitter, just get a 433.92Mhz one that are more abundantly available and replace the oscillator
with a 433.42MHz one (can be found on ebay).

Protocols
---------
Protocol support is still limited, but the code is set up for extensibility. Currently supported protocols:
* DIO Chacon (433.92MHz)
* Somfy (433.42MHz)