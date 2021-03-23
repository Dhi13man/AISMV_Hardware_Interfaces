#include <Arduino.h>

//#include "networking_backend.cpp"
#include "aismv_operation.cpp"

AISMVOperator *aismvController;
Pixy2I2C pixy;
//WebServerBackendInterface *backendInterface;
void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1);
  pixy.init();
  //pixy.setLamp(1024, 1024);
  aismvController = new AISMVOperator(pixy);
  // Network Backend Setup
  //backendInterface = new WebServerBackendInterface(*aismvController);
}

void loop() {
  aismvController->scanTarget();
}