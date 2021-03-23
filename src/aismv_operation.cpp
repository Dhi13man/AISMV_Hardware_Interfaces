#include <Arduino.h>

#include "machine_vision.cpp"
#include "soil_system.cpp"

class AISMVOperator {
private:
  String state;

  MVInterface *mvController;

  ObstacleInterface obstacleSenseController;

  /// Once this is true, nothing more is done.
  bool isFinalTargetReached;

  /// Needed to change direction every time a waterable target is seen after a non waterable target
  bool isPreviousTargetNonWaterable;

  /// Changes every time a waterable target is seen after a non waterable target.
  char scanDirection;

  bool scanDirectionForTime(String direction, unsigned long time = 1000) {
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
      else
        sabertoothController.stop();
    }
    return wasFound;
  }

  void moveTowardsTarget(Block &target) {
    state = "Moving towards target";
    if (target.m_x < 15)
      sabertoothController.hardLeft();
    else if (target.m_x > 275)
      sabertoothController.hardRight();
    else if (target.m_x >= 40 && target.m_x <= 275) {
      if (target.m_y < 190)
        sabertoothController.forward();
      else {
        // Get over the target and then act (WATER, CHANGE LANE or END)
        sabertoothController.forward();
        delay(2000);
        actBasedOnTargetType(target);
      }
    }
  }

  void actBasedOnTargetType(Block &target) {
    switch (target.m_signature) {
      // Target to be watered
      case 1:
        if (isPreviousTargetNonWaterable) {
          scanDirection = (scanDirection == 'l') ? 'r' : 'l';
          isPreviousTargetNonWaterable = false;
        }
        soilController.detectMoistureThenWater();
        break;
      
      // Target not to be watered. Lane changer
      case 2:
        isPreviousTargetNonWaterable = true;
        break;

      // Final Target reached
      case 3:
        isFinalTargetReached = true;
        return;
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
    isFinalTargetReached = false;
    isPreviousTargetNonWaterable = false;
    scanDirection = soilController.getFarmDirection();
    state = "Initialized";
  }

  void scanTarget() {
    if (isFinalTargetReached) {
      state = "Final Target Reached. Stopped";
      sabertoothController.stop();
      return;
    }

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

  String getState() {
    return state;
  }
};
