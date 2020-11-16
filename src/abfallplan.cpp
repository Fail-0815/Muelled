#include "abfallplan.h"

void updateEEPROM(const char* host, const uint8_t* fingerprint, const char* street_id){
    WiFiClientSecure client;
    client.setFingerprint(fingerprint);
    if(!client.connect(host, 443)){
      Serial.println("Connection to abfallkalender failed");
      delay(5000);
      return;
    }
    client.print(String("POST / HTTP/1.1\r\n")  +
                 "Host: " + host + "\r\n" +
                 "User-Agent: curl/7.71.1\r\n" +
                 "Accept: */*\r\n" +
                 "Content-Length: 14\r\n" +
                 "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
                 "street_id=" + String(street_id)
    );
    //unsigned long lasttime = millis();
    //while(!client.available() && millis() - lasttime < 1000)
    //  delay(1);
    //while(client.available()){
    //  // This skips the HTML reply header
    //  char readchar = client.read();
    //  // Serial.print(readchar);
    //}
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
        
        foundType = 0;
      }
      if(output == "</html>"){
        break;
      }
    }
    Serial.println("closing connection.");
}