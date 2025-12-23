#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ---------------- WIFI ----------------
const char* ssid = "GALGOTIAS-ARUBA";
const char* password = "1234567@";

WebServer server(80);

// ---------------- ADCs ----------------
Adafruit_ADS1115 adsSensors; // 0x48
Adafruit_ADS1115 adsE1;      // 0x49
Adafruit_ADS1115 adsE2;      // 0x4A
