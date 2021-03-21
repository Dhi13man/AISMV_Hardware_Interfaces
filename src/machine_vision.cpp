#include <Arduino.h>
#include <Pixy2I2C.h>

/// Interface controlling the machine vision system through SPI communication
///
/// SDA (Pixy 9) -> 21, SCL (Pixy 5) -> 22, Ground (Pixy 6) -> GND
class MVInterface {
  private:
  Pixy2I2C pixy;
  
  public:
  MVInterface(Pixy2I2C &usedPixy) {
    pixy = usedPixy;
  }

  /// Finds out how many blocks (targets) are detected by Pixy2.
  int getNumberOfTargets() {
    pixy.ccc.getBlocks();
    return pixy.ccc.numBlocks;
  }

  // Detect all blocks (targets) and finds the nearest target, by virtue of bounding box area
  Block detectAndGetNearest() {
    pixy.ccc.getBlocks();
    int largestArea = INT_MIN, largestAreaIndex = 0;
    for (int i = 0; i < pixy.ccc.numBlocks; ++i) {
      int area = pixy.ccc.blocks[i].m_height * pixy.ccc.blocks[i].m_width;
      if (area > largestArea) {
        largestAreaIndex = i;
        largestArea = area;
      }
    }
    return pixy.ccc.blocks[largestAreaIndex];
  }

  ~MVInterface() {
    pixy.~TPixy2();
  }

};


/// Depends on Ultrasound Sensor to avoid plants and obstacles in Farmland
class ObstacleInterface {
  private:
  /// Pins for using Ultrasound sensor for obstacle avoiding
  int ultrasoundEchoPin, ultrasoundTriggerPin;

  /// Compass Sensor for 
  int compassSensorPin;

  public:
  // Trigger -> Pink/red jumper
  ObstacleInterface(int ultrasoundEcho=32, int ultrasoundTrigger=33, int compassPin=35) {
    // Ultrasound
    ultrasoundTriggerPin = ultrasoundTrigger;
    pinMode(ultrasoundTriggerPin, OUTPUT);
    ultrasoundEchoPin = ultrasoundEcho;
    pinMode(ultrasoundEchoPin, INPUT);
    // Compass Sensor
    compassSensorPin = compassPin;
    pinMode(compassSensorPin, INPUT);
  }

  int getDistanceToObstacleInCentimeter() {
    digitalWrite(ultrasoundTriggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(ultrasoundTriggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultrasoundTriggerPin, LOW);
    pinMode(ultrasoundEchoPin, INPUT);
    int pulseDurationMicroSeconds = pulseIn(ultrasoundEchoPin, HIGH);
    return pulseDurationMicroSeconds / 29 / 2;;
  }
};
