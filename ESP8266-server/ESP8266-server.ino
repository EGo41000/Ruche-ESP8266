/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
#include <OneWire.h>

#define useWifi 1

const char* network[2] = {"XXX", "YYY"};

const char* host = "192.168.0.10";
const char* streamId   = "ruche2";
const char* privateKey = "FablabRH";
const char path[] = "/test/log.php";
const int httpPort = 80;

OneWire  ds(2);  // GPIO_1 (a 4.7K resistor is necessary)
  byte data[12];
  byte addr[8];

  byte addr1[8] = {0x28, 0x18, 0x91, 0x44, 0x06, 0x00, 0x00, 0xA1};
  byte addr2[8] = {0x28, 0xDD, 0x75, 0x44, 0x06, 0x00, 0x00, 0x10};
  float T1, T2;

float temp(byte *addr)
{
  byte data[12];
  byte i;

  ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad
  //Serial.print("  Data = ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.print(" adr1=");
  //Serial.print(addr[1], HEX);

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  float celsius = (float)raw / 16.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius");

  Serial.println();
  return celsius;
}

void setup() {
  Serial.begin(115200);
  delay(10);
  int n=0;
  
/*
  ds.reset_search();
  ds.search(addr);
  Serial.print("\nROM1 =");
  for(byte i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
  ds.search(addr);
  Serial.print("\nROM2 =");
  for(byte i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
*/

#if (useWifi)
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(network[0]);

  WiFi.begin(network[0], network[1]);
  
  while (WiFi.status() != WL_CONNECTED && (n++ < 10)) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

int value = 0;

void loop() {
  delay(5000);
  ++value;

  ds.reset();
  ds.write(0xCC);        // skip ROM
  ds.write(0x44);        // start conversion, with parasite power on at the end
  delay(1000); // 1 sec

  T1 = temp(addr1);
  T2 = temp(addr2);



#if (useWifi)
  Serial.print("connecting to ");
  Serial.println(host);
  
  // We now create a URI for the request
  String url = "/input/";
  url += streamId;
  url += "?private_key=";
  url += privateKey;
  //url += "&value=";
  //url += value;
  url += "&T1=";
  url += T1;
  url += "&T2=";
  url += T2;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
#endif

}

