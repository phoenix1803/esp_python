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
Adafruit_ADS1115 adsE3;      // 0x4B

// ---------------- TEMP SENSOR ----------------
#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// ---------------- CONFIG ----------------
int totalReadings = 7;
int currentReading = 1;
bool measuring = false;
unsigned long lastSample = 0;

// ---------------- DATA STRUCT ----------------
struct Sample {
  float uv, tds, ph;  
  float pt, cu, zn, ss, ag, graphite;
  float temp;
};

// ---------------- SENSOR READ ----------------
Sample readSensors() {
  const float LSB_sens = 0.0001875;
  const float LSB_elec = 0.0078125;

  Sample s;

  int16_t uv_raw  = adsSensors.readADC_SingleEnded(0);
  int16_t tds_raw = adsSensors.readADC_SingleEnded(1);
  int16_t ph_raw  = adsSensors.readADC_SingleEnded(2);

  s.uv  = uv_raw * LSB_sens;
  s.tds = tds_raw * LSB_sens;
  s.ph  = (ph_raw * LSB_sens) * 1.5;

  s.pt       = adsE1.readADC_Differential_0_1() * LSB_elec;
  s.cu       = adsE1.readADC_Differential_2_3() * LSB_elec;
  s.zn       = adsE2.readADC_Differential_0_1() * LSB_elec;
  s.ss       = adsE2.readADC_Differential_2_3() * LSB_elec;
  s.ag       = adsE3.readADC_Differential_0_1() * LSB_elec;
  s.graphite = adsE3.readADC_Differential_2_3() * LSB_elec;

  tempSensor.requestTemperatures();
  s.temp = tempSensor.getTempCByIndex(0);

  return s;
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  adsSensors.begin(0x48);
  adsE1.begin(0x49);
  adsE2.begin(0x4A);
  adsE3.begin(0x4B);

  adsSensors.setGain(GAIN_ONE);
  adsE1.setGain(GAIN_ONE);
  adsE2.setGain(GAIN_ONE);
  adsE3.setGain(GAIN_ONE);

  tempSensor.begin();

  Serial.println("✅ ESP32 SERIAL SENSOR STREAM STARTED");
}

// ---------------- MAIN LOOP ----------------
void loop() {
  if (millis() - lastSample >= 1000) {
    lastSample = millis();
    Sample s = readSensors();

    Serial.print("{");
    Serial.print("\"UV\":"); Serial.print(s.uv, 4);
    Serial.print(",\"TDS\":"); Serial.print(s.tds, 4);
    Serial.print(",\"pH\":"); Serial.print(s.ph, 4);
    Serial.print(",\"Pt\":"); Serial.print(s.pt, 3);
    Serial.print(",\"Cu\":"); Serial.print(s.cu, 3);
    Serial.print(",\"Zn\":"); Serial.print(s.zn, 3);
    Serial.print(",\"SS\":"); Serial.print(s.ss, 3);
    Serial.print(",\"Ag\":"); Serial.print(s.ag, 3);
    Serial.print(",\"Graphite\":"); Serial.print(s.graphite, 3);
    Serial.print(",\"Temp\":"); Serial.print(s.temp, 3);
    Serial.println("}");
  }
}