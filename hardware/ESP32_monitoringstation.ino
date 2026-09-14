// Include libraries
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>
#define ss 5
#define rst 14
#define dio0 2

// WiFi and MQTT variables setup
const char *ssid = "ssid_wifi"; // ganti menjadi nama wifi anda
const char *password = "password_wifi"; // ganti menjadi password wifi anda
const char *mqtt_broker = "123.456.789.123"; // ganti menjadi IP server anda
const char *topic = "ta_sensornode";
const char *mqtt_username = "mqtt_user"; // ganti menjadi username server anda
const char *mqtt_password = "mqtt_password"; // ganti menjadi password server anda
const int mqtt_port = 1883;

// Wifi and MQTT setup
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
// Initalize WiFi
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

// Initialize MQTT server connection
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while(!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.print("Connected to MQTT!");
    } else {
      Serial.print("failed with state: ");
      Serial.print(client.state());
      Serial.println();
      delay(500);
    }
  }
  client.subscribe(topic);

// Setup LoRa
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(915E6)) {
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
}

// MQTT callback function
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.println("Received message from topic!");
}

void loop() {
// Receive from LoRa and publish to MQTT
  int packetSize = LoRa.parsePacket();
  if(packetSize) {
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      client.publish(topic, LoRaData.c_str());
    }
  }
}
