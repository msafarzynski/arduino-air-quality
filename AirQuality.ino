#include <i2c.h>
#include <sensirion_common.h>
#include <sensirion_configuration.h>
#include <sgp30.h>
#include <sgp_featureset.h>

#include <DHT.h>
#include <LiquidCrystal.h>
#include <Seeed_HM330X.h>

#include "TempAndHumidity.cpp"

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL_OUTPUT SerialUSB
#else
#define SERIAL_OUTPUT Serial
#endif

#define LCD_SCROLLS 4
#define BOOT_TIME 2000
#define HUM_PIN 11
#define BUTTON_PIN 12

typedef enum {
  SENSOR_NUM = 1,
  PM1_STANDARD = 2,
  PM2_5_STANDARD = 3,
  PM10_STANDARD = 4,
  PM1 = 5,
  PM2_5 = 6,
  PM10 = 7
} PM_READING;

struct GasReading {
    uint16_t co2;
    uint16_t tvoc;
}; 

DHT dht;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
HM330X particleSensor;
TempAndHumidity tempAndHumidity;

int buttonPrev = LOW;
int currentScrollLine = 0;
unsigned long previousMillisSensors = 0;
unsigned long previousMillisLcd = 0;

uint8_t buf[30];
const char* str[] = {
  "sensor num: ",
  "PM1.0 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
  "PM2.5 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
  "PM10 concentration(CF=1,Standard particulate matter,unit:ug/m3): ",
  "PM1.0 concentration(Atmospheric environment,unit:ug/m3): ",
  "PM2.5 concentration(Atmospheric environment,unit:ug/m3): ",
  "PM10 concentration(Atmospheric environment,unit:ug/m3): ",
};


void setup() {
  // SERIAL_OUTPUT.begin(9600);
  dht.setup(HUM_PIN, dht.DHT22);
  lcd.begin(16, 3);
  setupGasSensor();

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  SERIAL_OUTPUT.begin(115200);
  delay(100);
  SERIAL_OUTPUT.println("Serial start");
  if (particleSensor.init()) {
    SERIAL_OUTPUT.println("HM330X init failed!!");
    while (1)
      ;
  }
}

void loop() {
  unsigned long currentMillis = millis();
  float temperature;
  float avgTemperature;
  float humidity;
  float avgHumidity;
  uint16_t pm1;
  uint16_t pm2_5;
  uint16_t pm10;
  char CELCIUS_SYMBOL = (char)223;


  String temperatureDesc = tempAndHumidity.getTemperatureDescription(avgTemperature);
  String humidityDesc = tempAndHumidity.getHumidityDescription(avgHumidity);
  GasReading gasReading = readGasSensor();


  if (currentMillis < BOOT_TIME) {
    lcd.clear();
    lcd.print("Booting");

    delay(BOOT_TIME);
  }

  if (currentMillis - previousMillisSensors >= dht.getMinimumSamplingPeriod()) {
    previousMillisSensors = currentMillis;

    temperature = dht.getTemperature();
    avgTemperature = tempAndHumidity.calculateAvgTemp(temperature);
    temperatureDesc = tempAndHumidity.getTemperatureDescription(avgTemperature);

    humidity = dht.getHumidity();
    avgHumidity = tempAndHumidity.calculateAvgHumidity(humidity);
    humidityDesc = tempAndHumidity.getHumidityDescription(avgHumidity);

    if (particleSensor.read_sensor_value(buf, 29)) {
      SERIAL_OUTPUT.println("HM330X read result failed!!");
    }
    parse_result_value(buf);
    parse_result(buf);

    pm1 = getPmReading(buf, PM1);
    pm2_5 = getPmReading(buf, PM2_5);
    pm10 = getPmReading(buf, PM10);


    SERIAL_OUTPUT.println((String) "temp: " + temperature);
    SERIAL_OUTPUT.println((String) "avg temp: " + avgTemperature + "C " + temperatureDesc);

    SERIAL_OUTPUT.println((String) "humidity: " + humidity);
    SERIAL_OUTPUT.println((String) "avg humidity: " + avgHumidity + "% " + humidityDesc);

    SERIAL_OUTPUT.println((String) "PM1.0: " + pm1);
    SERIAL_OUTPUT.println((String) "PM2.5: " + pm2_5);
    SERIAL_OUTPUT.println((String) "PM10: " + pm10);
  }

  if (currentMillis - previousMillisLcd >= 500) {
    previousMillisLcd = currentMillis;

    lcd.clear();
    lcd.setCursor(0, 0 - 2*currentScrollLine);
    lcd.print(String(avgTemperature, 1) + CELCIUS_SYMBOL + "C " + temperatureDesc);
    lcd.setCursor(0, 1 - 2*currentScrollLine);
    lcd.print(String(avgHumidity) + "% " + humidityDesc);
    lcd.setCursor(0, 2 - 2*currentScrollLine);
    lcd.print((String) "PM1.0: " + pm1 + "ug/m3");
    lcd.setCursor(0, 3 - 2*currentScrollLine);
    lcd.print((String) "PM2.5: " + pm2_5 + "ug/m3");
    lcd.setCursor(0, 4 - 2*currentScrollLine);
    lcd.print((String) "PM10: " + pm10 + "ug/m3");
    
    lcd.setCursor(0, 6 - 2*currentScrollLine);
    lcd.print((String) "CO2: " + gasReading.co2 + "ppm");
    lcd.setCursor(0, 7 - 2*currentScrollLine);
    lcd.print((String) "PM10: " + gasReading.tvoc + "ppb");
  }

  evalScrollButton();
}

void evalScrollButton() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState != buttonPrev && buttonState) {
    currentScrollLine = (currentScrollLine + 1) % LCD_SCROLLS;
    digitalWrite(LED_BUILTIN, buttonState);
    lcd.clear();
  }

  buttonPrev = buttonState;
}

