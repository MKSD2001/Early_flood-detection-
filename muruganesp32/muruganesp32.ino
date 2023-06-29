#include <WiFi.h>
#include <WiFiClient.h>
#include "twilio.hpp"

const char* ssid = "Heram";
const char* password = "87654321";
static const char* account_sid = "AC52809cfed56b22c4e37019a75c79cae6";
static const char* auth_token = "d2c6778188b977c30c9b1e66897d7986";
static const char* from_number = "+12705889326"; // Phone number should start with "+<countrycode>"
static const char* to_number = "+917305447894";   // Phone number should start with "+<countrycode>"

#define RXD2 16 // ESP32 RX pin
#define TXD2 17 // ESP32 TX pin

WiFiClient wifiClient;
float distance = 0.0;
float rainLevel = 0.0;
Twilio* twilio = nullptr;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println(WiFi.localIP());

  twilio = new Twilio(account_sid, auth_token);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (Serial2.available() > 0) {
      String data = Serial2.readStringUntil('\n');

      if (data.startsWith("Rain level:")) {
        rainLevel = data.substring(12).toFloat();
        Serial.print("Received rain level: ");
        Serial.print(rainLevel);
        Serial.println("%");
      } else if (data.startsWith("Distance:")) {
        distance = data.substring(10).toFloat();
        Serial.print("Received distance: ");
        Serial.print(distance);
        Serial.println("cm");
      }
    }

    String msgBody = "";
    String mediaUrl = "";

    if (distance < 10) {
      msgBody = "Dam water level has reached its limit: ";
      msgBody += String(distance, 1) + " cm\n";
      msgBody += "Flood Alert! Take necessary precautions.";
    } 
    else if(distance < 8){
      msgBody = "DOOR OPENING... : ";
      msgBody += String(distance, 1) + " cm\n";
      msgBody += "Flood Alert! Take necessary precautions.";
      
    }
    
    else if (rainLevel > 30) {
      msgBody = "Rainfall is too high: ";
      msgBody += String(rainLevel, 1) + "%\n";
      msgBody += "Heavy rainfall expected. Stay safe!";
    }

    if (!msgBody.equals("")) {
      bool success = twilio->send_message(to_number, from_number, msgBody, mediaUrl);
      if (success) {
        Serial.println("Sent message successfully!");
      } else {
        Serial.println("Failed to send message.");
      }
    }
  }
}
