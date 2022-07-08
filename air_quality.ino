#include <DHT.h>
#include <LiquidCrystal.h>
#include <lcd.cpp>
#include <temp_and_humidity.cpp>

DHT dht;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
const int HUM_PIN = 11;
const int TEMP_PIN = 0;
const int BUTTON_PIN = 12;
const int SUPPLY_VOLTAGE = 5;
const String celciusDegree = String((char)223) + 'C';
int buttonPrev = LOW;
bool a = 0;
const int humTableSize = 10;
const int tempTableSize = 10;
int humTable[humTableSize] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float tempTable[tempTableSize] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

void setup()
{
  Serial.begin(9600);
  dht.setup(HUM_PIN);
  lcd.begin(16, 3);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  float temp = 0.5 * round(2.0 * (analogRead(TEMP_PIN) * (float)SUPPLY_VOLTAGE / 1024) * 100);
  int hum = dht.getHumidity();
  int avgHum = calculateHumidity(hum);
  float avgTemp = calculateAvgTemp(temp);

  Serial.println(String(hum));
  Serial.print("\t");
  Serial.println(String(avgHum));
  Serial.print("\t\t");
  Serial.println(temp);
  Serial.print("\t\t");
  Serial.println(avgTemp);

  lcd.clear();
  lcd.setCursor(0, 0 - a);
  lcd.print(String(temp, 1) + celciusDegree + " " + evalTempStatus(temp));
  lcd.setCursor(0, 1 - a);
  lcd.print(String(avgTemp, 1) + celciusDegree + " " + evalTempStatus(avgTemp));
  lcd.setCursor(0, 2 - a);
  lcd.print(String(avgHum) + "%    " + evalHumStatus(avgHum));

  evalButton();

  delay(dht.getMinimumSamplingPeriod());

}

float calculateAvgTemp(float temp) {
  pushToTempArray(temp);
  float avgTemp = 0.5 * round(2.0 * (calculateAvgTemp()));
  return avgTemp;
}

void pushToTempArray(float temp){
  for (int i = 0; i < tempTableSize - 1; i++) {
    tempTable[i] = tempTable[i + 1];
  }
  tempTable[tempTableSize - 1] = temp;
}

float calculateAvgTemp(){
  float sum = 0;
  int nonZeroCount = 0;
  for (int i = 0; i < tempTableSize; i++) {
    sum += tempTable[i];
    if (tempTable[i] != 0) {
      nonZeroCount++;
    }
  }
  if (nonZeroCount == 0) {
    return 0;
  }
  return sum / nonZeroCount;
}

int calculateHumidity(int hum) {
  pushToHumArray(hum);
  int avgHum = calculateHumAvg();
  return avgHum;
}

void pushToHumArray(int hum) {
  if (hum == 0) {
    return;
  }

  for (int i = 0; i < humTableSize - 1; i++) {
    humTable[i] = humTable[i + 1];
  }
  humTable[humTableSize - 1] = hum;
}

int calculateHumAvg() {
  int sum = 0;
  int nonZeroCount = 0;
  for (int i = 0; i < humTableSize; i++) {
    sum += humTable[i];
    if (humTable[i] != 0) {
      nonZeroCount++;
    }
  }
  if (nonZeroCount == 0) {
    return 0;
  }
  return sum / nonZeroCount;
}

void evalButton() {
  int buttonState = digitalRead(BUTTON_PIN);
  bool ledState = 0;

  if (buttonState != buttonPrev && buttonState) {
    Serial.print("INNNN!!!");
    a = !a;
    digitalWrite(LED_BUILTIN, a);
    lcd.clear();
  }
  buttonPrev = buttonState;
}

String evalTempStatus(float temp) {
  if (temp < 14) {
    return "Too cold";
  }
  if (temp >= 14 && temp < 17) {
    return "Cold";
  }
  if (temp >= 17 && temp < 19) {
    return "Ideal";
  }
  if (temp >= 19 && temp < 22) {
    return "Warm";
  }
  return "Too warm";
}

String evalHumStatus(int hum) {
  if (hum < 30) {
    return "Too dry";
  }
  if (hum >= 30 && hum < 40) {
    return "Dry";
  }
  if (hum >= 40 && hum < 60) {
    return "Ideal";
  }
  if (hum >= 60 && hum <= 70) {
    return "Humid";
  }
  return "Too humid";
}
