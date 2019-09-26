// Internet de las Cosas / Internet of Things / www.internetdelascosas.cl
// Sketch to connect a QuakeAlarm device using an ESP8266
// Version 1.0 ESP8266
// Author: Jose Antonio Zorrilla @joniuz
// 2019-09-26 @joniuz

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Network Configuration
#define WIFISSID "your-wifi-ssid" // Put here your Wi-Fi SSID
#define PASSWORD "your-wifi-password" // Put here your Wi-Fi

// QuakeAlarm Configuration
#define ID "37b1d8f04dea14a8decbd34e1e923579" // This ID is unique for each device, ask for one to contact@iot.cl
#define SERVER "dcs.iot.cl" // This is the iot.cl server
#define QUAKEALARM A0 // Input pin - This pin must be an anlog input

// QuakeAlarm internal variables
int qaValue = 0; // QuakeAlarm current value
int qaPreviousValue = 0; // QuakeAlarm previous value
int sensibility = 10; // QuakeAlarm sensibility
int difference = 0; // Diference between to values
long ping = 10000; // Initial interval for ping requests
int timeInterval = 10000; // Time interval between activation requests
long lastConnection = 0; // Last activation request
int lastStatus = 0; // Status of the last connection
long lastPing = 0; // Last ping request

ESP8266WiFiMulti WiFiMulti;

// Function to extract the interger parameters from the XML payload
int xmlTakeParam(String inStr,String needParam)
{ 
  if(inStr.indexOf("<"+needParam+">")>=0){
     int CountChar=needParam.length();
     
     int indexStart=inStr.indexOf("<"+needParam+">");
     int indexStop= inStr.indexOf("</"+needParam+">");  
     return inStr.substring(indexStart+CountChar+2, indexStop).toInt();
  }
  return 0;
}

// Function to send data to the server
void httpRequest(char action[200]) {

  if ((WiFiMulti.run() == WL_CONNECTED)) {
    Serial.println("QuakeAlarm: Connecting to the server...");
    Serial.print("WiFiMulti.run() = ");
    Serial.println(WiFiMulti.run());

    char url[200];
    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");

    if (action == "ping") {
      sprintf(url, "http://%s/sensor.php?id=%s&sensor=quakealarm&valor=ping&value=%d", SERVER, ID, qaValue);
      lastPing = millis();
      Serial.println("[HTTP] Start a ping request");
    }
    else {
      sprintf(url, "http://%s/sensor.php?id=%s&sensor=quakealarm&valor=%d&diferencia=%d", SERVER, ID, qaValue, difference);
      lastConnection = millis();
      Serial.println("[HTTP] Start a activation request");
    }
    
    if (http.begin(client, url )) {  // HTTP

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // Process the payload sent by the server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.print("Payload     : ");
          Serial.println(payload);
          sensibility = xmlTakeParam(payload,"sensibilidad");
          Serial.print("Sensibility : ");
          Serial.println(sensibility);
          ping = xmlTakeParam(payload,"ping");
          Serial.print("Ping        : ");
          Serial.println(ping);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  else {
    // Wifi connection problems
    Serial.print("WiFiMulti.run() = ");
    Serial.println(WiFiMulti.run());
    // Wait one second
    delay(1000);
  }
}

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[Setup] Wait %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFISSID, PASSWORD);

}

void loop() {
  // Get the QuakeAlarm value
  qaValue = analogRead(QUAKEALARM);

  // Get the difference with the previous value
  difference = qaPreviousValue - qaValue;

  // Send an activation request to the server
  if ((difference > 0) and (difference >= sensibility) and ((millis() - lastConnection) > timeInterval)) {
    httpRequest("activation");
  }

  // Send a ping request to the server
  if ((millis() - lastPing) > ping) {
    httpRequest("ping");
  }
}
