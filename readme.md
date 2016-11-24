RfCom
=====

General purpose Arduino based 433MHz transmitter. Supports 433.42MHz and 433.92MHz transmitters on pin 5 and pin 6
respectively (configured in "common.h").

To get a 433.42MHz transmitter, just get a 433.92Mhz one that are more abundantly available and replace the oscillator
with a 433.42MHz one (can be found on ebay).

Protocols
---------
Protocol support is still limited, but the code is set up for extensibility. Currently supported protocols:
* 1: Chacon DIO (433.92MHz)
* 2: Somfy (433.42MHz)

Usage
-----
The device can be configured and controlled through the serial port. When first using the device, you'll have to
initialize it to set up the configuration data structures. You can do so by issuing the "initialize" command.

After that you can add devices. Each device will need to be given an id, a protocol id (the number as shown in the
'Protocols' section of this document) and a configuration string. Use the "add_device" command to add them. Devices
are stored in EEPROM and will update their configuration automatically if necessary (e.g. for rolling codes).

Finally you can send commands to a device using the "send" command.

Example
-------
```
> initialize
< Erasing...Done!
> add_device 2 1 1,12325261
< Success
> send 2 on
```
