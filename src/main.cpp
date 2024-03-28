#include <Arduino.h>
#include <FS.h>
#include "MHZ19.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "SSID";
const char* password = "*********";
const char* mqtt_server = "192.168.178.XXX"; //add IP of MQTT server
int port = 1883;
//if you have several devices you have to adjust the name
char DevName[] = "CO2";
char output[] ="CO2/telemetry";

#define RX_PIN 5         //MH-Z19 RX-PIN                                         
#define TX_PIN 4         //MH-Z19 TX-PIN                                   
#define BAUDRATE 9600    //Terminal Baudrate
//Klassen definierung
MHZ19 myMHZ19;                               
SoftwareSerial s(RX_PIN, TX_PIN); 
unsigned long lastSend;

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
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (client.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        client.subscribe(output);
      }
      
  }
}


  void callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageM;
    
    for (int i = 0; i < length; i++) {
      Serial.print((char)message[i]);
      messageM += (char)message[i];
    }
    Serial.println();
  }

void setupMQTT() {
  client.setServer(mqtt_server , port);
  // set the callback function
  client.setCallback(callback);
}



void setup() {
    Serial.begin(9600);
  setup_wifi();
setupMQTT();    
  // InitWiFi();
  s.begin(BAUDRATE);                            //Serial for MH-Z19 CO2 Sensor  
myMHZ19.begin(s);                                // *Serial(Stream) refence must be passed to library begin(). 

myMHZ19.autoCalibration();     
  lastSend = 0;
}

void loop() {
     if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if ( millis() - lastSend > 5000 ) { // Update and send only after 5 seconds
    int CO2 = myMHZ19.getCO2();
client.publish(output, String(CO2).c_str());

    lastSend = millis();
  }


}
