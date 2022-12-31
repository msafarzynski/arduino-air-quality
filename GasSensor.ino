#include "SparkFun_SGP30_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_SGP30
#include <Wire.h>

SGP30 sgp30Sensor; //create an object of the SGP30 class

void setupGasSensor() {
  Serial.begin(9600);
  Wire.begin();
  //Initialize sensor
  if (sgp30Sensor.begin() == false) {
    Serial.println("No SGP30 Detected. Check connections.");
    while (1);
  }
  //Initializes sensor for air quality readings
  //measureAirQuality should be called in one second increments after a call to initAirQuality
  sgp30Sensor.initAirQuality();
}

GasReading readGasSensor() {
  //First fifteen readings will be
  //CO2: 400 ppm  TVOC: 0 ppb
  delay(1000); //Wait 1 second
  //measure CO2 and TVOC levels
  sgp30Sensor.measureAirQuality();
  Serial.print("CO2: ");
  Serial.print(sgp30Sensor.CO2);
  Serial.print(" ppm\tTVOC: ");
  Serial.print(sgp30Sensor.TVOC);
  Serial.println(" ppb");

  GasReading gasReading;

  gasReading.co2 = sgp30Sensor.CO2;
  gasReading.tvoc = sgp30Sensor.TVOC;

  return gasReading;
//get raw values for H2 and Ethanol
    // sgp30Sensor.measureRawSignals();
    // Serial.print("Raw H2: ");
    // Serial.print(sgp30Sensor.H2);
    // Serial.print(" \tRaw Ethanol: ");
    // Serial.println(sgp30Sensor.ethanol);

}