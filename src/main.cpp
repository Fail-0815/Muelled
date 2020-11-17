#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <abfallplan.h>

const char* ssid = "marburg.freifunk.net";
// const char* passphrase = "";
const char* host = "marburg.mein-abfallkalender.de";

const char* street_id = "6374";

const uint8_t fingerprint[20] = {0x4F, 0x5C, 0xCF, 0x3B, 0xB7, 0x0E, 0xD1, 0x18, 0xA0, 0x7B, 0xB3, 0x13, 0x92, 0xC3, 0x16, 0x82, 0xD7, 0x44, 0x4F, 0x98};

bool data_needed = true;

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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());
  Serial.println();
  setup_abfall(host, fingerprint, street_id);
}

void loop() {
  if(data_needed){
    abfalltermin x = next_termin();
    Serial.println(String(x.date) + String(x.type));
    data_needed = false;
  }
  delay(5000);
}