#include <DHT.h>
#include <LiquidCrystal.h>
#include <Seeed_HM330X.h>

#include "TempAndHumidity.cpp"

typedef enum {
  SENSOR_NUM = 1,
  PM1_STANDARD = 2,
  PM2_5_STANDARD = 3,
  PM10_STANDARD = 4,
  PM1 = 5,
  PM2_5 = 6,
  PM10 = 7
} PM_READING;

DHT dht;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
HM330X sensor;
TempAndHumidity tempAndHumidity;

const int HUM_PIN = 11;
const int TEMP_PIN = 0;
const int BUTTON_PIN = 12;
const int SUPPLY_VOLTAGE = 5;
const int BOOT_TIME = 2000;
const int LCD_REFRESH_RATE = 500;
const int LCD_SCROLLS = 3;
const char CELCIUS_SYMBOL = (char)223;

int buttonPrev = LOW;
int currentScrollLine = 0;
unsigned long previousMillisSensors = 0;
unsigned long previousMillisLcd = 0;

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL_OUTPUT SerialUSB
#else
#define SERIAL_OUTPUT Serial
#endif

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
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  SERIAL_OUTPUT.begin(115200);
  delay(100);
  SERIAL_OUTPUT.println("Serial start");
  if (sensor.init()) {
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
  String temperatureDesc = tempAndHumidity.getTemperatureDescription(avgTemperature);
  String humidityDesc = tempAndHumidity.getHumidityDescription(avgHumidity);
  uint16_t pm1;
  uint16_t pm2_5;
  uint16_t pm10;

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

    if (sensor.read_sensor_value(buf, 29)) {
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

HM330XErrorCode print_result(const char* str, uint16_t value) {
  if (NULL == str) {
    return ERROR_PARAM;
  }
  SERIAL_OUTPUT.print(str);
  SERIAL_OUTPUT.println(value);
  return NO_ERROR;
}

/*parse buf with 29 uint8_t-data*/
HM330XErrorCode parse_result(uint8_t* data) {
  uint16_t value = 0;
  if (NULL == data) {
    return ERROR_PARAM;
  }
  for (int i = 1; i < 8; i++) {
    value = (uint16_t)data[i * 2] << 8 | data[i * 2 + 1];
    print_result(str[i - 1], value);
  }

  return NO_ERROR;
}

uint16_t getPmReading(uint8_t* data, int pmReading) {
  if (NULL == data) {
    return ERROR_PARAM;
  }

  return (uint16_t)data[pmReading * 2] << 8 | data[pmReading * 2 + 1];
}

HM330XErrorCode parse_result_value(uint8_t* data) {
  if (NULL == data) {
    return ERROR_PARAM;
  }
  for (int i = 0; i < 28; i++) {
    SERIAL_OUTPUT.print(data[i], HEX);
    SERIAL_OUTPUT.print("  ");
    if ((0 == (i) % 5) || (0 == i)) {
      SERIAL_OUTPUT.println("");
    }
  }
  uint8_t sum = 0;
  for (int i = 0; i < 28; i++) {
    sum += data[i];
  }
  if (sum != data[28]) {
    SERIAL_OUTPUT.println("wrong checkSum!!");
  }
  SERIAL_OUTPUT.println("");
  return NO_ERROR;
}