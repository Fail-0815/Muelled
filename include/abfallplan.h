#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <string.h>
#include <EEPROM.h>
#include <TimeLib.h>

typedef uint8_t abfalltype;
struct abfalltermin {
    abfalltype type;
    time_t date;
};
class Abfallplan{
    const char* host;
    const uint8_t* fingerprint;
    const char* streetid;
    uint8_t counter;
    const char* wastefilter = "&filter_waste%3Alist=137&filter_waste%3Alist=138&filter_waste%3Alist=139&filter_waste%3Alist=148&filter_waste%3Alist=136&filter_waste%3Alist=140&filter_waste%3Alist=141";

    private:
        void updateEEPROM();
        time_t epoch_from_date(String date);
        time_t read_date_at_offset(int offset);
        abfalltype char_to_abfalltype(char input); 

    public: 
        void setup_abfall(const char* host, const uint8_t* fingerprint, const char* street_id);
        void setup_abfall(const char* host, const uint8_t* fingerprint, const char* street_id, const char* wastefilter);
        abfalltermin next_termin();
};