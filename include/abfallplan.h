#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <string.h>
#include <EEPROM.h>
#include <TimeLib.h>

struct abfalltermin {
    uint8_t type;
    time_t date;
};
class Abfallplan{
    const char* host;
    const uint8_t* fingerprint;
    const char* streetid;
    uint8_t counter;

    private:
        void updateEEPROM();
        time_t epoch_from_date(String date);

    public: 
        void setup_abfall(const char* host, const uint8_t* fingerprint, const char* street_id);
        abfalltermin next_termin();
};