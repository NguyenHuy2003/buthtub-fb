#include "DHT.h"
#include <ESP32Servo.h>
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <LittleFS.h>
#include <SD.h>
#include <Update.h>

#define DHTPIN 13
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

Servo panas;
Servo dingin;
int pos = 0;
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
// WiFi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Firebase configuration
const char* FIREBASE_HOST = "buthtub-firebase-default-rtdb.firebaseio.com";
const char* FIREBASE_AUTH = "tGTdtPHtflw95j7ZM4WSfTRtzLQDcAxtQAzudEem";
const char* databaseURL = "https://buthtub-firebase-default-rtdb.firebaseio.com/data.json";

void setup() {
  panas.attach(5);
  dingin.attach(2);
  Serial.begin(9600);
  dht.begin();
  delay(100);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {
  // Read temperature and humidity
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  Serial.print("Water temperature: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);
  delay(1000);

  // Send data to Firebase using HTTP
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(databaseURL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", FIREBASE_AUTH);

    String data = String("{\"temperature\":") + String(t) + String(", \"humidity\":") + String(h) + String("}");
    int httpResponseCode = http.PUT(data);

    if (httpResponseCode > 0) {
      Serial.printf("Data sent successfully, response code: %d\n", httpResponseCode);
    } else {
      Serial.printf("Error sending data, response code: %d\n", httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  Serial.println("______________________________");
 for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees

    // in steps of 1 degree

    panas.write(pos);              // tell servo to go to position in variable 'pos'

    delay(15);                       // waits 15ms for the servo to reach the position

  }

  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees

    panas.write(pos);              // tell servo to go to position in variable 'pos'

    delay(15);        
}
}