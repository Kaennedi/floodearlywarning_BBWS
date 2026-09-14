// Variables setup
#define rainsensor_pin 13

// Non-blocking timers and callables
int rainsensor_minutely;
int rainsensor_count;
int rainsensor_status;
float sensortimer;
float sendtimer;

// Interrupt function
void rainsensor() {
  sensortimer = millis();
  rainsensor_status = 1;
}

void setup() {
// Initialize non-blocking timers
  Serial.begin(115200);
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

// Send serial data to Raspberry Pi datalogger block
  if ((millis() - sensortimer) >= 60000 && rainsensor_status == 2) {
    Serial.print(sensortimer, 3);
    Serial.print(";");
    Serial.print(rainsensor_minutely);
    Serial.print(";");
    Serial.print(rainsensor_count);
    Serial.println();
    rainsensor_minutely = 0;
    rainsensor_status = 0;
    sendtimer = millis();
  }
}
