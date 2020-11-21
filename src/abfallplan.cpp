#include "abfallplan.h"
#define DEBUG

void Abfallplan::setup_abfall(const char* hostname, const uint8_t* ssl_fingerprint, const char* abfall_streetid){
  host = hostname;
  fingerprint = ssl_fingerprint;
  streetid = abfall_streetid;
  EEPROM.begin(512);
  #ifdef DEBUG
  EEPROM.write(0, 0);
  #endif
  counter = EEPROM.read(0);
  Serial.println("Counter on startup: " + String(counter));
}

void Abfallplan::setup_abfall(const char* host, const uint8_t* fingerprint, const char* street_id, const char* waste_filter){
  wastefilter = waste_filter;
  setup_abfall(host, fingerprint, street_id);
}

time_t Abfallplan::epoch_from_date(String date){
  time_t result = 0;
  TimeElements Time;
  Time.Second = 0;
  Time.Minute = 0;
  Time.Hour   = 0;
  Time.Day    = date.substring(0,2).toInt();
  Time.Month  = date.substring(3,5).toInt();
  Time.Year   = date.substring(6,10).toInt() - 1970;
  result = makeTime(Time);
  return result;
}

abfalltype Abfallplan::char_to_abfalltype(char input){
  abfalltype ret = 0;
  switch (input){
    case 'P': // Altpapier, theoretisch auch Sperrmüll, wird aber durch die Filter in der Abfrage ignoriert (Müsste vorher eh angemeldet werden)
      ret = 0b00000001;
      break;
    case 'A': // Sonderabfall. IdR nicht abgefragt (Ohnehin fester Termin in Mdmaburg)
      ret = 0b00000010;
      break;
    case 'B': // Weihnachtsbaum.
      ret = 0b00000100;
      break;
    case '"': // Restmüll (2 wöchentlich)
      ret = 0b00001000;
      break;
    case '4': // Restmüll (4 wöchentlich)
      ret = 0b00010000;
      break;
    case 'i': // Biomüll
      ret = 0b00100000;
      break;
    case 'S': // Gelber Sack
      ret = 0b01000000;
      break;
    case 'R': // Grünschnitt. IdR nicht abgefragt.
      ret = 0b10000000;
      break;
    default:
      ret = 0;
      Serial.println("Strange input recieved: " + String(input));
  }
  return ret;
}

time_t Abfallplan::read_date_at_offset(int offset){
  time_t epochdate = 0;
  for(unsigned int i = 0; i < sizeof(time_t); i++){
    uint8_t temp = EEPROM.read(offset + 1 + i);
    epochdate += ((time_t) temp) << (i * 8);
  }
  return epochdate;
}

void Abfallplan::updateEEPROM(){
  Serial.println("updateEEPROM called");
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);
  if(!client.connect(host, 443)){
    Serial.println("Connection to abfallkalender failed");
    delay(5000);
    return;
  }
  String request = String("POST / HTTP/1.1\r\n")  +
               "Host: " + host + "\r\n" +
               "User-Agent: curl/7.71.1\r\n" +
               "Accept: */*\r\n" +
               "Content-Length: 204\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
               "street_id=" + String(streetid) + "&filter_period=next_10" + wastefilter;
  client.print(request);

  counter = 10;
  uint8_t tmpcount = counter;
  char foundType = 0;
  while(String output = client.readStringUntil('\n')){
    if(strstr(output.c_str(),"\"waste_type\"")){
      // The div class waste_type was found
      foundType = output[42];
      continue;
    }
    if (foundType > 0 && (output.indexOf('.') >= 0)){
      // The next line containg a dot after a waste_type will be the date
      time_t epochdate = epoch_from_date(output.substring(10,20));
      tmpcount -= 1;
      uint8_t offset = tmpcount * (1 + sizeof(time_t)) + 1;
      EEPROM.write(offset, (uint8_t) char_to_abfalltype(foundType));
      for(unsigned int i = 0; i < sizeof(time_t); i++){
        uint8_t tmp = epochdate >> (i * 8);
        EEPROM.write(offset + 1 + i, tmp);
      }
      foundType = 0;
      continue;
    }
    if(output == "</html>"){
      break;
    }
  }
  EEPROM.write(0, counter);
  Serial.println("closing connection.");
}

abfalltermin Abfallplan::next_termin(){
  Serial.println("next_termin called");
  if(counter == 0){
    updateEEPROM();
  }
  uint8_t offset = (counter-1) * (1 + sizeof(time_t)) + 1; // 1 byte type, 10 bytes date (Strings in EEPROM are NOT \0 terminated) plus 1 for the counter byte in the beginning
  //Serial.println("Reading from Slot: " + String(counter - 1));
  abfalltermin output;
  uint8_t temptype = EEPROM.read(offset);
  output.type = temptype;
  output.date = read_date_at_offset(offset);
  for(int i = 1; i <= counter; )
  return output;
}