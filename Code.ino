#define USE_ARDUINO_INTERRUPTS true 
#include <PulseSensorPlayground.h> 
#include <SimpleDHT.h>
int a[20], i = 0;
int j, sum = 0;
const int PulseWire = 0; 
const int LED13 = 13; 
int Threshold = 250; 
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);
PulseSensorPlayground pulseSensor; 
void setup() {

  Serial.begin(9600); 
  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED13); 
  pulseSensor.setThreshold(Threshold);
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !"); 
  }
}

void loop() {

  int myBPM = pulseSensor.getBeatsPerMinute();
  
  if (pulseSensor.sawStartOfBeat()) { 
    Serial.print("BPM: "); 
    Serial.println(myBPM);
    a[i % 20] = myBPM;
    for (j = 0; j < 20; j++)
    {
      sum += a[j];
    }
    if (sum >= 110)
      Serial.println("Alert");
    digitalWrite(13, HIGH);
    i++;
     // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  
  // DHT11 sampling rate is 1HZ.
  delay(1500);
  }
  //delay(10);
}
