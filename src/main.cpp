#include <Arduino.h>

#include "networking_backend.cpp"

AISMVOperator *aismvController;
WebServerBackendInterface *backendInterface;


 void setup() {
  Serial.begin(9600);
  Pixy2I2C pixy; 
  //pixy.init();
  //pixy.setLamp(1024, 1024);
  aismvController = new AISMVOperator(pixy);
  backendInterface = new WebServerBackendInterface(*aismvController);
  // Network Backend Setup

}

void loop() {
  //aismvController->scanTarget();
}