#include <WString.h>

using namespace std;

static const int QUEUE_SIZE = 10;

class TempAndHumidity {
private:
  float temperatureArray[QUEUE_SIZE] = {};
  float humidityArray[QUEUE_SIZE] = {};

  float calculateAvg(float sensorValues[]) {
    float sum = 0;
    for (int i = 0; i < QUEUE_SIZE; i++) {
      sum += sensorValues[i];
    }
    return sum / QUEUE_SIZE;
  }

  float * pushToQueue(float * sensorValues, float currentSensorValue) {
    for (int i = 0; i < QUEUE_SIZE - 1; i++) {
      sensorValues[i] = sensorValues[i + 1];
    }
    sensorValues[QUEUE_SIZE - 1] = currentSensorValue;

    return sensorValues;
  }

public:
  float calculateAvgTemp(float temp) {
    pushToQueue(temperatureArray, temp);

    return calculateAvg(temperatureArray);
  }

  float calculateAvgHumidity(float humidity) {
    pushToQueue(humidityArray, humidity);

    return calculateAvg(humidityArray);
  }

  String getTemperatureDescription(float temp) {
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

  String getHumidityDescription(float hum) {
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
};
