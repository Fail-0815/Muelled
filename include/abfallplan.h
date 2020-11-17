#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <string.h>
#include <EEPROM.h>

struct abfalltermin {
    uint8_t type;
    char* date;
};

void updateEEPROM();

void setup_abfall(const char* host, const uint8_t* fingerprint, const char* street_id);

abfalltermin next_termin();