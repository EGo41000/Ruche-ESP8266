Implementation d'un module pour une ruche.

La puce est un ESP8266.
Arduino IDE 1.6.3 avec addition de Arduino-ESP8266 (https://github.com/sandeepmistry/esp8266-Arduino)

2xDS18b20 connectés broche GPIO2 (OneWire  ds(2)):
  byte addr1[8] = {0x28, 0x18, 0x91, 0x44, 0x06, 0x00, 0x00, 0xA1};
  byte addr2[8] = {0x28, 0xDD, 0x75, 0x44, 0x06, 0x00, 0x00, 0x10};

ESP8266-server: scketch basé sur les exemples.