

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_GPS.h> //Load the GPS Library. Make sure you have installed the library form the adafruit site above
#include <SoftwareSerial.h> //Load the Software Serial Library. This library in effect gives the arduino additional serial ports
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <SPI.h>
#include <SD.h>


#define DHTPIN 2     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)


int a[20], i = 0;
int j, sum = 0;
const int PulseWire = 0;
const int LED13 = 13;
int Threshold = 250;
PulseSensorPlayground pulseSensor;
String pulse;
String temper;

SoftwareSerial mySerial(3, 2); //Initialize SoftwareSerial, and tell it you will be connecting through pins 2 and 3
Adafruit_GPS GPS(&mySerial); //Create GPS object

String NMEA1;  //We will use this variable to hold our first NMEA
String NMEA2;  //We will use this variable to holdsentence our second NMEA sentence
char c;       //Used to read the characters spewing from the GPS module
File myFile;





DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

void setup() {
  Serial.begin(9600);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //Tell GPS we want only $GPRMC and $GPGGA NMEA sentences
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);   // 1 Hz update rate
  delay(1000);


  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;


  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED13);
  pulseSensor.setThreshold(Threshold);
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");
  }



}


void temperature()
{
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    temper = String(event.temperature, 10);
    Serial.println(F("°C"));
  }


}




void readGPS() { //This function will read and remember two NMEA sentences from GPS
  clearGPS();    //Serial port probably has old or corrupt data, so begin by clearing it all out
  while (!GPS.newNMEAreceived()) { //Keep reading characters in this loop until a good NMEA sentence is received
    c = GPS.read(); //read a character from the GPS
  }
  GPS.parse(GPS.lastNMEA());  //Once you get a good NMEA, parse it
  NMEA1 = GPS.lastNMEA();    //Once parsed, save NMEA sentence into NMEA1
  while (!GPS.newNMEAreceived()) { //Go out and get the second NMEA sentence, should be different type than the first one read above.
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  NMEA2 = GPS.lastNMEA();
  Serial.println(NMEA1);
  Serial.println(NMEA2);
  Serial.println("");
}




void clearGPS() {  //Since between GPS reads, we still have data streaming in, we need to clear the old data by reading a few sentences, and discarding these
  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());

}




void pulserate()
{

  int myBPM = pulseSensor.getBeatsPerMinute();
  pulse = String(myBPM, 10);

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
    //digitalWrite(13, HIGH);
    i++;
    delay(1000);
  }

}




void sdcard_write(String str)
{

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println(str);
    myFile.println(" ");
    myFile.close();
    Serial.println("done.");
  }

  else {
    Serial.println("error opening test.txt");
  }



}


void sdcard_read()
{
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}


void storesd()
{

  sdcard_write(NMEA1);
  sdcard_write(" ");
  sdcard_write(NMEA2);
  sdcard_write(" \n");
  sdcard_write("pulserate");
  sdcard_write(":");
  sdcard_write(pulse);
  sdcard_write(" \n");
  sdcard_write("temperture");
  sdcard_write(":");
  sdcard_write(temper);
  sdcard_write(" C ");
  sdcard_write("\n");

}

void loop() {

  temperature();
  readGPS();
  pulserate();
  storesd();



}
