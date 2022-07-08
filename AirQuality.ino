#include <DHT.h>
#include <LiquidCrystal.h>
// #include "Lcd.cpp"
#include "TempAndHumidity.cpp"

DHT dht;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
TempAndHumidity tempAndHumidity;

const int HUM_PIN = 11;
const int TEMP_PIN = 0;
const int BUTTON_PIN = 12;
const int SUPPLY_VOLTAGE = 5;
const int BOOTING_TIME = 2000;
const String celciusDegree = String((char)223) + 'C';

int buttonPrev = LOW;
bool a = 0;
unsigned long previousMillisSensors = 0;
unsigned long previousMillisLcd = 0;


void setup() {
  Serial.begin(9600);
  dht.setup(HUM_PIN);
  lcd.begin(16, 3);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  float temp;
  float avgTemp;
  int hum;
  int avgHum;

  if (currentMillis < BOOTING_TIME) {
    lcd.clear();
    lcd.print("Booting");

    delay(BOOTING_TIME);
  }

  if (currentMillis - previousMillisSensors >= 2000) {
    previousMillisSensors = currentMillis;

    temp = dht.getTemperature();
    avgTemp = tempAndHumidity.calculateAvgTemp(temp);
    hum = dht.getHumidity();
    avgHum = tempAndHumidity.calculateHumidity(hum);

    Serial.println((String) "humidity: " + hum);
    Serial.println((String) "avg humidity: " + avgHum);
    Serial.println((String) "temp: " + temp);
    Serial.println((String) "avg temp: " + avgTemp);
  }

  if (currentMillis - previousMillisLcd >= 500) {
    previousMillisLcd = currentMillis;

    lcd.setCursor(0, 0 - a);
    lcd.print(String(avgTemp, 1) + celciusDegree + " " + tempAndHumidity.evalTempStatus(avgTemp));
    lcd.setCursor(0, 1 - a);
    lcd.print(String(avgHum) + "%    " + tempAndHumidity.evalHumStatus(avgHum));
  }

  evalButton();
}

void evalButton() {
  int buttonState = digitalRead(BUTTON_PIN);
  bool ledState = 0;

  if (buttonState != buttonPrev && buttonState) {
    a = !a;
    digitalWrite(LED_BUILTIN, a);
    lcd.clear();
  }
  buttonPrev = buttonState;
}