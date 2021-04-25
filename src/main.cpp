#include <Arduino.h>

#include "networking_backend.cpp"

// Library Elements
Pixy2I2C pixy;
// Self defined Source elements.
AISMVOperator *aismvController;
WebServerBackendInterface *backendInterface;

void setup() {
  Serial.begin(9600);
  // Set up PIXY2 Machine vision camera and turn its flash on.
  pixy.init();
  pixy.setLamp(255, 255);
  // AISMV Setup
  aismvController = new AISMVOperator(pixy);
  // Network Backend Setup
  backendInterface = new WebServerBackendInterface(*aismvController);

}

void loop() {
  aismvController->scanTarget();
}