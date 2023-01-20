#include <Wire.h>
#include "Protocentral_MAX30205.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

MAX30205 tempSensor;
//SoftwareSerial mySerial(5,6);

int buzzPin = 8;  // declare variable for the store buzzer pin 
String DataSendESP;

void setup() {
  pinMode(buzzPin, OUTPUT);
  Serial.begin(115200);
  Wire.begin();

  //scan for temperature in every 30 sec untill a sensor is found. Scan for both addresses 0x48 and 0x49
  while(!tempSensor.scanAvailableSensors()){
    Serial.println("Couldn't find the temperature sensor, please connect the sensor." );
    delay(30000);
  }
  tempSensor.begin();   // set continuos mode, active mode
}

void loop() {
	float temperature = tempSensor.getTemperature(); // read temperature for every 100ms
	if (temperature > 0){
    // Send data to ESP
    DataSendESP = "";
    DataSendESP = DataSendESP + temperature;
    Serial.println(DataSendESP);
    delay(5000);
  }
  if(temperature > 38.5){
      digitalWrite(buzzPin, HIGH);
      delay(3000);
      digitalWrite(buzzPin, LOW);
      delay(3000);
   }
}
