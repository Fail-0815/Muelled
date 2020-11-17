#include "abfallplan.h"
#define DEBUG
// TODO: make this a class

const char* host_;
const uint8_t* fingerprint_;
const char* streetid;
uint8_t counter;
const char* wastefilter = "&filter_waste%3Alist=137&filter_waste%3Alist=138&filter_waste%3Alist=139&filter_waste%3Alist=148&filter_waste%3Alist=136&filter_waste%3Alist=140&filter_waste%3Alist=141";

void setup_abfall(const char* hostname, const uint8_t* ssl_fingerprint, const char* abfall_streetid){
  host_ = hostname;
  fingerprint_ = ssl_fingerprint;
  streetid = abfall_streetid;
  EEPROM.begin(512);
  #ifdef DEBUG
  EEPROM.write(0, 0);
  #endif
  counter = EEPROM.read(0);
  Serial.println("Counter on startup: " + String(counter));
}

void updateEEPROM(){
  Serial.println("updateEEPROM called");
  WiFiClientSecure client;
  client.setFingerprint(fingerprint_);
  if(!client.connect(host_, 443)){
    Serial.println("Connection to abfallkalender failed");
    delay(5000);
    return;
  }
  String request = String("POST / HTTP/1.1\r\n")  +
               "Host: " + host_ + "\r\n" +
               "User-Agent: curl/7.71.1\r\n" +
               "Accept: */*\r\n" +
               "Content-Length: 204\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
               "street_id=" + String(streetid) + "&filter_period=next_10" + wastefilter;
  Serial.println(request);
  client.print(request);
  //unsigned long lasttime = millis();
  //while(!client.available() && millis() - lasttime < 1000)
  //  delay(1);
  //while(client.available()){
  //  // This skips the HTML reply header
  //  char readchar = client.read();
  //  // Serial.print(readchar);
  //}
  EEPROM.write(0, 10);
  counter = 10;
  uint8_t tmpcount = counter;
  char foundType = 0;
  while(String output = client.readStringUntil('\n')){
    //Serial.println(output);
    if(strstr(output.c_str(),"\"waste_type\"")){
      // The div class waste_type was found
      foundType = output[42];
      Serial.println("Found Type: " + String(foundType));
      continue;
    }
    if (foundType > 0 && (output.indexOf('.') >= 0)){
      // The next line containg a dot after a waste_type will be the date
      String date = output.substring(10,20);
      Serial.println(date);
      tmpcount -= 1;
      uint8_t offset = tmpcount * (1 + 10) + 1;
      EEPROM.write(offset, (uint8_t) foundType);
      for(int i = 0; i < 10; i++)
        EEPROM.write(offset + 1 + i, (uint8_t) date[i]);
      foundType = 0;
      Serial.println("Wrote to EEPROM Slot: " + String(tmpcount));
    }
    if(output == "</html>"){
      break;
    }
  }
  Serial.println("closing connection.");
}

abfalltermin next_termin(){
  Serial.println("next_termin called");
  if(counter == 0){
    updateEEPROM();
  }
  uint8_t offset = (counter-1) * (1 + 10) + 1; // 1 byte type, 10 bytes date (Strings in EEPROM are NOT \0 terminated) plus 1 for the counter byte in the beginning
  abfalltermin output;
  output.type = EEPROM.read(offset);
  char* date = (char*)malloc(11); // 11 byte address + \0
  date[10] = '\0';
  for(int i = 0; i < 10; i++)
    date[i] = EEPROM.read(offset + 1 + i);
  output.date = date;
  return output;
}