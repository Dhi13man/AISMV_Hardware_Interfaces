#include <Arduino.h>

#include "machine_vision.cpp"
#include "soil_system.cpp"

class AISMVOperator {
  private:
  String state;

  MVInterface *mvController;

  ObstacleInterface obstacleSenseController;

  bool scanDirectionForTime(String direction, unsigned long time=1000) {
      state = "Scanning: " + direction;
      bool wasFound = false;
      // Operation begins
      unsigned long initialTime = millis();
      while ((millis() - initialTime) <= time) {
        if (mvController->getNumberOfTargets() > 0) {
            wasFound = true;
            break;
        }
        if (direction.compareTo("hardLeft") == 0)
          sabertoothController.hardLeft();
        else if (direction.compareTo("hardRight") == 0)
          sabertoothController.hardRight();
        else if (direction.compareTo("smoothLeft") == 0)
          sabertoothController.smoothLeft();
        else if (direction.compareTo("smoothRight") == 0)
          sabertoothController.smoothRight();
        else if (direction.compareTo("reverse") == 0)
           sabertoothController.reverse();
        else sabertoothController.stop();
      }
      return wasFound;
  }

  void moveTowardsTarget(Block target) {
    state = "Moving towards target";
    if (target.m_x < 15)
      sabertoothController.hardLeft();
    else if (target.m_x > 275)
      sabertoothController.hardRight();
    else if (target.m_x >= 40 && target.m_x <= 275) {
      if (target.m_y < 190)
        sabertoothController.forward();
      else {
        sabertoothController.stop();
        delay(500);
        sabertoothController.forward();
        delay(1000);
        if (target.m_signature == 1)
          soilController.detectMoistureThenWater();
      }
    }
  }

  public:
  SoilInterface soilController;

  SabertoothInterface sabertoothController;

  AISMVOperator(Pixy2I2C &pixy) {
    // Initialize Controllers
    mvController = new MVInterface(pixy);
    sabertoothController = SabertoothInterface();
    soilController = SoilInterface(15, 2, 12);
    obstacleSenseController = ObstacleInterface(32, 33, 35);
    state = "Initialized";
  }

  void scanTarget(char scanDirection = 'l') {
    state = "Scanning Target";
    bool isTargetFound = false;
    // Scan in Front
    isTargetFound = scanDirectionForTime("forward", 1000);
    if (!isTargetFound && scanDirection == 'l') {
        if (!isTargetFound)
          isTargetFound = scanDirectionForTime("hardLeft", 1000);
        if (!isTargetFound)
          isTargetFound = scanDirectionForTime("hardRight", 2000);
        if (!isTargetFound)
          isTargetFound = scanDirectionForTime("hardLeft", 1000);
    }
    else if (!isTargetFound && scanDirection == 'r') {
        if (!isTargetFound)
          isTargetFound = scanDirectionForTime("hardRight", 1000);
        if (!isTargetFound)
          isTargetFound = scanDirectionForTime("hardLeft", 2000);
        if (!isTargetFound)
          isTargetFound = scanDirectionForTime("hardRight", 1000);
    }

    // Operate Differently based on the signature of target
    if (isTargetFound) {
      Block nearestTarget = mvController->detectAndGetNearest();
      moveTowardsTarget(nearestTarget);
    }
  }

  ~AISMVOperator() {
    sabertoothController.~SabertoothInterface();
  }
};

