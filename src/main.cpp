#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <abfallplan.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


const char* ssid = "marburg.freifunk.net";
// const char* passphrase = "";
const char* host = "marburg.mein-abfallkalender.de";

const char* street_id = "6374";

const uint8_t fingerprint[20] = {0x4F, 0x5C, 0xCF, 0x3B, 0xB7, 0x0E, 0xD1, 0x18, 0xA0, 0x7B, 0xB3, 0x13, 0x92, 0xC3, 0x16, 0x82, 0xD7, 0x44, 0x4F, 0x98};

bool data_needed = true;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, 60 * 60 * 24); // Update Interval of a day. We dont need precise timing.
Abfallplan AbfallplanClient;

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
  AbfallplanClient.setup_abfall(host, fingerprint, street_id);
}

void loop() {
  timeClient.update();
  time_t now = timeClient.getEpochTime();
  // Serial.println(sizeof(long)); // 4
  if(data_needed){
    abfalltermin x = AbfallplanClient.next_termin();
    Serial.println(x.date, 16);
    Serial.println(String(day(x.date)) + "." + String(month(x.date)) + "." + String(year(x.date)) + " Typ:" + String(x.type));
    data_needed = false;
  }
  delay(5000);
}