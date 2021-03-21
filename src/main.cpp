#include <Arduino.h>

#include "aismv_operation.cpp"

AISMVOperator *aismvController;
Pixy2I2C pixy; 

 void setup() {
  Serial.begin(9600);
  pixy.init();
  //pixy.setLamp(1024, 1024);
  aismvController = new AISMVOperator(pixy);
  // Network Backend Setup
  //backendInterface = WebServerBackendInterface(aismvController);
}

void loop() {
  aismvController->scanTarget();
}