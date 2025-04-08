#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "env.h"

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Connect to WiFi
  if (IS_WOKWI) {
    WiFi.begin(SSID, PASS, CHANNEL);
  } else {
    WiFi.begin(SSID, PASS);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // GET request to /api/light
    HTTPClient http;
    http.begin("http://" + String(ENDPOINT) + "/api/light");
    http.addHeader("api-key", API_KEY);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      StaticJsonDocument<200> doc;
      deserializeJson(doc, payload);
      bool lightState = doc["light"];
      digitalWrite(LED_PIN, lightState ? HIGH : LOW);
      Serial.println("Light state: " + String(lightState));
    } else {
      Serial.println("Error on GET request");
    }
    http.end();

 
    int tempValue = analogRead(TEMP_SENSOR);
    float voltage = tempValue * (3.3 / 4095.0);
    float temperature = voltage * 100; // LM35 sensor

    String tempData = "{\"temp\": " + String(temperature) + "}";
    http.begin("http://" + String(ENDPOINT) + "/api/temp");
    http.addHeader("api-key", API_KEY);
    http.addHeader("Content-Type", "application/json");

    httpCode = http.PUT(tempData);
    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.println("Temperature sent: " + String(temperature));
      Serial.println("API Response: " + response);
    } else {
      Serial.println("Error on PUT request");
    }
    http.end();
  }
  delay(5000); //  5 seconds delay
}