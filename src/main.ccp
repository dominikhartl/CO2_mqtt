#include <Arduino.h>
#include <FS.h>
#include <mDNS.h>
#include <ArduinoOTA.h>
/*
   Measure KH: k
   measure pH: p
   fill titration tube: f
   measure sample volume: s
   measure titration volume: t
   start stirrer: m
   end stirrer: e
   remove sample: r
   Calibration:
     c enterph -> enter the calibration mode
     c calph   -> calibrate with the standard buffer solution, two buffer solutions(4.0 and 7.0) will be automaticlly recognized
     c exitph  -> save the calibrated parameters and exit from calibration mode

*/

// Replace the next variables with your SSID/Password combination
const char* ssid = "HartlwLan";
const char* password = "DoBiMiLi";
//if you have several devices you have to adjust the name
char DevName[] = "CO2";
char output[] ="CO2/telemetry";

String clientId = "CO2"
const char  *User = "CO2";
const char  *Password = "pwd";

const char* mqtt_server = "192.168.178.23";
int        port     = 1883;

#include <Arduino.h>
#include "MHZ19.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ThingsBoard.h>


#define RX_PIN 5         //MH-Z19 RX-PIN                                         
#define TX_PIN 4         //MH-Z19 TX-PIN                                   
#define BAUDRATE 9600    //Terminal Baudrate
//Klassen definierung
MHZ19 myMHZ19;                               
SoftwareSerial mySerial(RX_PIN, TX_PIN); 
unsigned long getDataTimer = 0;


#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!client.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      
      if (client.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        client.subscribe(output);
      }
      
  }
}




void setup() {
  setup_wifi();
  client.setServer(mqtt_server , port);

  // InitWiFi();
  myMHZ19.begin(mySerial);
  lastSend = 0;
}

void loop() {
  if ( !tb.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 5000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  tb.loop();
}


client.publish(DevName, "First drops fast!");