#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "AAsus-X550Z";
const char* password = "1234567890";
// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
char kirim[40];
float pres = 0.25;

void connectToWiFi() {
  Serial.print("Connectiog to ");
 
  WiFi.begin(ssid, password);
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected.");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println(" ");
}

void setupMQTT(){
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-"; 
      clientId += String(random(0xffff), HEX);
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("/bojar/receive");
      }  
  }
}

void setup(){
  Serial.begin(9600);
  connectToWiFi();
  setupMQTT();
  
}

void loop(){
  if (!mqttClient.connected()){
    reconnect();
  }
  sprintf(kirim, "%f", pres);
  mqttClient.publish("/bojar/send", kirim);
  pres++;
  delay(2000);
  mqttClient.publish("/bojar/send", "wow");
  delay(3000);
  mqttClient.loop();
}
