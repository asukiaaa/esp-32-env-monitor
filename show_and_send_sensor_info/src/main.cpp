#include <Arduino.h>
#include <AM2320_asukiaaa.h>
#include <ST7032_asukiaaa.h>
#include <ThingSpeakWriter_asukiaaa.h>

#define WIFI_SSID "****"
#define WIFI_PASS "****"
#define WIFI_CONNECT_WAIT_SEC 10

#define THINGSPEAK_WRITE_API_KEY     "****"
#define THINGSPEAK_TEMPERATURE_FIELD 1
#define THINGSPEAK_HUMIDITY_FIELD    2
#define THINGSPEAK_SEND_INTERVAL_MS  (10 * 60 * 1000)

AM2320_asukiaaa sensor;
ST7032_asukiaaa lcd;
ThingSpeakWriter_asukiaaa channelWriter(THINGSPEAK_WRITE_API_KEY);
unsigned long wroteAt = 0;

void connectToWifi() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long startAt = millis();
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(String(WiFi.status()));
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to WiFi");
      break;
    }
    if (millis() - startAt > 1000UL * WIFI_CONNECT_WAIT_SEC) {
      Serial.println("Cannot connect to WiFi");
      break;
    }
  }
}

void setup() {
  lcd.begin(8, 2);
  lcd.setContrast(30);
}

void loop() {
  connectToWifi();
  lcd.clear();
  if (sensor.update() != 0) {
    Serial.println("Cannot read value from sensor.");
    lcd.setCursor(0, 0);
    lcd.print("error");
  } else {
    Serial.println("TemperatureC: " + String(sensor.temperatureC));
    Serial.println("Humidity: " + String(sensor.humidity));
    lcd.setCursor(0, 0);
    lcd.print(sensor.temperatureC);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print(sensor.humidity);
    lcd.print("%");

    if (wroteAt == 0 || millis() - wroteAt > THINGSPEAK_SEND_INTERVAL_MS) {
      channelWriter.setField(THINGSPEAK_TEMPERATURE_FIELD, String(sensor.temperatureC));
      channelWriter.setField(THINGSPEAK_HUMIDITY_FIELD, String(sensor.humidity));
      int writeResult = channelWriter.writeFields();
      if (writeResult == 200) {
        Serial.println("Succeeded in writing values.");
        wroteAt = millis();
      } else {
        Serial.println("Failed to write. Error: " + String(writeResult));
      }
    }
  }
  delay(1000);
}
