#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <string.h>

void updateEEPROM(const char* host, const uint8_t* fingerprint, const char* street_id);