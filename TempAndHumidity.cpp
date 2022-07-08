// #include <DHT.h>
#include <WString.h>
using namespace std;

// DHT dht;
// const int HUM_PIN = 11;
// const int TEMP_PIN = 0;


class TempAndHumidity {
private:
  static const int humTableSize = 10;
  static const int tempTableSize = 10;
  int humTable[humTableSize] = {};
  float tempTable[tempTableSize] = {};

  float calculateAvgTemp() {
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

  void pushToTempArray(float temp) {
    for (int i = 0; i < tempTableSize - 1; i++) {
      tempTable[i] = tempTable[i + 1];
    }
    tempTable[tempTableSize - 1] = temp;
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

public:
  float calculateAvgTemp(float temp) {
    pushToTempArray(temp);
    return calculateAvgTemp();
  }

  int calculateHumidity(int hum) {
    pushToHumArray(hum);
    return calculateHumAvg();
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
};


// float TempAndHumidity::calculateAvgTemp(float temp) {
//   pushToTempArray(temp);
//   return calculateAvgTemp();
// }

// void TempAndHumidity::pushToTempArray(float temp){
//   for (int i = 0; i < tempTableSize - 1; i++) {
//     tempTable[i] = tempTable[i + 1];
//   }
//   tempTable[tempTableSize - 1] = temp;
// }

// float TempAndHumidity::calculateAvgTemp(){
//   float sum = 0;
//   int nonZeroCount = 0;
//   for (int i = 0; i < tempTableSize; i++) {
//     sum += tempTable[i];
//     if (tempTable[i] != 0) {
//       nonZeroCount++;
//     }
//   }
//   if (nonZeroCount == 0) {
//     return 0;
//   }
//   return sum / nonZeroCount;
// }

// int TempAndHumidity::calculateHumidity(int hum) {
//   pushToHumArray(hum);
//   return calculateHumAvg();
// }

// void TempAndHumidity::pushToHumArray(int hum) {
//   if (hum == 0) {
//     return;
//   }

//   for (int i = 0; i < humTableSize - 1; i++) {
//     humTable[i] = humTable[i + 1];
//   }
//   humTable[humTableSize - 1] = hum;
// }

// int TempAndHumidity::calculateHumAvg() {
//   int sum = 0;
//   int nonZeroCount = 0;
//   for (int i = 0; i < humTableSize; i++) {
//     sum += humTable[i];
//     if (humTable[i] != 0) {
//       nonZeroCount++;
//     }
//   }
//   if (nonZeroCount == 0) {
//     return 0;
//   }
//   return sum / nonZeroCount;
// }

// String TempAndHumidity::evalTempStatus(float temp) {
//   if (temp < 14) {
//     return "Too cold";
//   }
//   if (temp >= 14 && temp < 17) {
//     return "Cold";
//   }
//   if (temp >= 17 && temp < 19) {
//     return "Ideal";
//   }
//   if (temp >= 19 && temp < 22) {
//     return "Warm";
//   }
//   return "Too warm";
// }

// String TempAndHumidity::evalHumStatus(int hum) {
//   if (hum < 30) {
//     return "Too dry";
//   }
//   if (hum >= 30 && hum < 40) {
//     return "Dry";
//   }
//   if (hum >= 40 && hum < 60) {
//     return "Ideal";
//   }
//   if (hum >= 60 && hum <= 70) {
//     return "Humid";
//   }
//   return "Too humid";
// }