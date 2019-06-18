#include <Arduino.h>
#include <AM2320_asukiaaa.h>
#include <esp_deep_sleep.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "env_params.h"

#ifdef WITH_LCD
#include <ST7032_asukiaaa.h>

ST7032_asukiaaa lcd;
#endif

#define WIFI_RETRY_MAX_COUNT 10
#define SENSOR_SCAN_MAX_COUNT 10

const int sleepSeconds = 10 * 60;

AM2320 am2320;
int count = 0;

void setLed(boolean power) {
  digitalWrite(LED_PIN, power);
}

void goToSleep() {
  setLed(false);
  esp_deep_sleep_enable_timer_wakeup(sleepSeconds * 1000 * 1000);
  esp_deep_sleep_start();
}

void setup() {
  pinMode(OUTPUT, LED_PIN);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_deep_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);

  WiFi.begin(ssid, password);
  count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting to WiFi.." + String(WiFi.status()));
    if (count > WIFI_RETRY_MAX_COUNT) {
      goToSleep();
    }
    ++count;
  }
  Wire.begin();
  am2320.setWire(&Wire);
  count = 0;
#ifdef WITH_LCD
  lcd.setWire(&Wire);
  lcd.begin(8,2);
  lcd.setContrast(30);
#endif
}

void loop() {
  setLed(true);
  if (am2320.update() != 0) {
    Serial.println("Error: Cannot update sensor values.");
  } else {
    Serial.println("temperatureC: " + String(am2320.temperatureC) + " C");
    Serial.println("temperatureF: " + String(am2320.temperatureF) + " F");
    Serial.println("humidity: " + String(am2320.humidity) + " %");

    HTTPClient http;
    http.begin("https://api.thingspeak.com/update?api_key=" + THINGSPEAK_API_KEY +
               "&field" + String(TEMPERATURE_FIELD_NUM) + "=" + String(am2320.temperatureC) +
               "&field" + String(HUMIDITY_FIELD_NUM) + "=" + String(am2320.humidity));
    int httpCode = http.GET();
    if (httpCode > 0) {
#ifdef WITH_LCD
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(String(am2320.temperatureC) + "C");
      lcd.setCursor(0,1);
      lcd.print(String(am2320.humidity) + "%");
#endif
      Serial.println("sended sensor values");
      goToSleep();
    } else {
      Serial.println("failed to send sensor values");
    }
  }
  Serial.println("at " + String(millis()) + " ms");
  delay(1000);

  if (count > SENSOR_SCAN_MAX_COUNT) {
    goToSleep();
  }
  ++count;
}
