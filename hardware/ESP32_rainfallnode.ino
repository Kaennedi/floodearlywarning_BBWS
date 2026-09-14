// Include libraries
#include <TinyGPS++.h>
#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>

// Variables setup
#define rainsensor_pin 13
#define ss 5
#define rst 14
#define dio0 2
#define DHTPIN 12 //masukkan pin digital DHT11
#define DHTTYPE DHT11

// GPS setup
TinyGPSPlus gps;
DHT dht(DHTPIN, DHTTYPE);

// Non-blocking timers and callables
int rainsensor_minutely;
int rainsensor_count;
int rainsensor_status;
float sensortimer;
float sendtimer;

// Define variables
float temp;
float hum;

// Interrupt function
void rainsensor() {
  sensortimer = millis();
  rainsensor_status = 1;
}

void setup() {
// Setup LoRa
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(915E6)) {
    delay(500);
  }
  LoRa.setSyncWord(0xF3);

// Initialize non-blocking timers
  sensortimer = millis();
  sendtimer = millis();
  rainsensor_minutely = 0;
  rainsensor_count = 0;
  rainsensor_status = 0;

// Setup interrupt
  pinMode(rainsensor_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(rainsensor_pin), rainsensor, RISING);
}

void loop() {
// Post-interrupt processing block
  if (rainsensor_status == 1) {
    rainsensor_minutely += 1;
    rainsensor_count += 1;
    rainsensor_status == 2;
  }

// LoRa block
  if ((millis() - sensortimer) >= 60000 && rainsensor_status == 2 && Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      if (gps.location.isValid()) {
        temp = dht.readTemperature();
        hum = dht.readHumidity();
        LoRa.beginPacket();
        LoRa.print(gps.location.lat(), 3);
        LoRa.print(";");
        LoRa.print(gps.location.lng(), 3);
        LoRa.print(";");
        LoRa.print(sensortimer, 3);
        LoRa.print(";");
        LoRa.print(rainsensor_minutely);
        LoRa.print(";");
        LoRa.print(rainsensor_count);
        LoRa.print(";");
        LoRa.print(temp);
        LoRa.print(";");
        LoRa.print(hum);
        LoRa.println();
        LoRa.endPacket();
        rainsensor_minutely = 0;
        rainsensor_status = 0;
        sendtimer = millis();
      }
    }
  } else if ((millis() - sendtimer) > 5000 && rainsensor_status == 2 && gps.charsProcessed() < 10) {
    LoRa.beginPacket();
    LoRa.println("No GPS data! Check wiring!");
    LoRa.endPacket();
  }
}
