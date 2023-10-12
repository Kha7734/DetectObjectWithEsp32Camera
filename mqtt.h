#ifndef _MQTT_H_
#define _MQTT_H_

#include <PubSubClient.h>
#include <WiFi.h>
#include "Security.h"

bool AllowMovement = false;

/* Web defines ------------------------------------------------------------- */
const char* ssid = "Thao Nguyen";
const char* password = "0398876444";
const char* mqttServer = "broker.hivemq.com"; 
const int MQTTport = 1883;

/* App defines -------------------------------------------------------------- */
// https://maker.ifttt.com/trigger/export/with/key/d28fOLRWQKyFH1Rqrfq7Z5+?value1=Gas can&value2=1
const char* appHost = "maker.ifttt.com";
const int port = 80;
const char* label = "Gas can";
char* event = "export"; // Event name from your IFTTT applet
const char* apiKey = "d28fOLRWQKyFH1Rqrfq7Z5"; // Your IFTTT Webhooks API key

const char* cloudHost = "api.thingspeak.com";


// https://api.thingspeak.com/channels/2242939/fields/1.json?api_key=T5S2KK6348XX5R72&results=1

/* Global variables -------------------------------------------------------- */
int ObjectQuantity = 0;

// Client 
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//-------------------------------------------------------------------------/

void sendRequest(const char* customHost, double result, const char* event) {
  WiFiClient client;

  while (!client.connect(customHost, port)) {
    Serial.println("connection fail");
    delay(1000);
  }

  String request;
  if (strcmp(customHost, appHost) == 0) {
    // Construct the request for the app host
    request = "/trigger/" + String(event) + "/with/key/" + String(apiKey) + "?value1=" + String(label) + "&value2=" + String(result);
  } else if (strcmp(customHost, cloudHost) == 0) {
    // Construct the request for the web host
    // request = "/update?api_key=M0V9CSXLA54K0UM7&field1=" + String(result);
    request = "/update?api_key=3VET6N42F49S5YE9&field1=" + String(result);
  }

  String httpRequest = String("GET ") + request + " HTTP/1.1\r\n"
                    + "Host: " + customHost + "\r\n"
                    + "Connection: close\r\n\r\n";
  Serial.println("Request:\n " + httpRequest);
  client.print(httpRequest);
  delay(500);

  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}

void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void mqttConnect() {
  while(!mqttClient.connected()) {
    Serial.println("Attemping MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if(mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");

      mqttClient.subscribe("Tool/PIRsensor");
    }
    else {
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.println(topic);
  String strMsg;
  for(int i = 0; i < length; i++) 
    strMsg += (char)message[i];
  
  Serial.println(strMsg);
  ei_printf("%s", strMsg);
  if (strMsg == "true")
        AllowMovement = true;
  if (strMsg == "false")
        AllowMovement = false;
}

void setupMqtt() {
    wifiConnect();
    mqttClient.setServer(mqttServer, MQTTport);
    mqttClient.setCallback(callback);
    mqttClient.setKeepAlive(90);
}

void loopMqtt()
{ 
  if(!mqttClient.connected()) 
    mqttConnect();
  mqttClient.loop();

}

#endif