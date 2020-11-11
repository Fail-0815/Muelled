#include <Arduino.h>
#include <ESP8266WiFi.h>

const char* ssid = "marburg.freifunk.net";
// const char* passphrase = "";
const char* host = "marburg.mein-abfallkalender.de";

const char* street_id = "6374";

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);
  // WiFi.begin(ssid, passphrase);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}