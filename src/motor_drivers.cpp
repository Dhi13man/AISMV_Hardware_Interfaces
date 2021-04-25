#include <Arduino.h>

/// Controlling interface class for Sabertooth motor driver through SERIAL communication.
class SabertoothInterface {
  private:
  String systemState;

  /// Sends commands to Sabertooth to move Motor 
  ///
  /// motorNumber -> [1 -> Left Motor, 2 -> Right Motor]; [speed] -> (-1024, 1024)
  void moveMotor(int motorNumber, int speed) {
    // Constraint speed value for Sabertooth
    if (speed > 1023) speed = 1023;
    if (speed < -1023) speed = -1023;

    String command;
    if (motorNumber == 1) command += "M1: ";
    else command += "M2: ";
    command += String(speed, DEC) + "\r\n";
    Serial2.print(command.c_str());
    //Serial.print(command.c_str());
  }

  public:
  SabertoothInterface() {
    // For Serial Communication.
    Serial2.begin(9600, SERIAL_8N1); // Responsible for communication with Sabertooth
    systemState = "Initialized";
  }

  void smoothRight(int speed=350) {
    moveMotor(1, speed);
    moveMotor(2, 0);
    systemState = "Moving Right Smoothly";
  }

  void smoothLeft(int speed=350) {
    moveMotor(2, -speed);
    moveMotor(1, 0);
    systemState = "Moving Left Smoothly";
  }

  void hardRight(int speed=350) {
    moveMotor(1, speed);
    moveMotor(2, speed);
    systemState = "Moving Hard Right";
  }

  void hardLeft(int speed=350) {
    moveMotor(2, -speed);
    moveMotor(1, -speed);
    systemState = "Moving Hard Left";
  }

  void forward(int speed=400) {
    moveMotor(1, speed);
    moveMotor(2, -speed);
    systemState = "Moving Forward";
  }

  void reverse(int speed=400) {
    moveMotor(1, -speed);
    moveMotor(2, speed);
    systemState = "Moving Reverse";
  }

  void stop() {
    moveMotor(1, 0);
    moveMotor(2, 0);
    systemState = "Stopped";
  }

  String getState() {
    return systemState;
  }
};

/// Controlling interface class for Motor driver controlling Water pump
class WaterPumpMotorDriverInterface {
  private:
  int motor1_forward_pin, motor1_backward_pin;

  public:
  WaterPumpMotorDriverInterface(int forward_pin=12, int backward_pin=14) {
    motor1_forward_pin = forward_pin;
    motor1_backward_pin = backward_pin;
    pinMode(motor1_backward_pin, OUTPUT);
    pinMode(motor1_forward_pin, OUTPUT);
  }

  void forward() {
    digitalWrite(motor1_forward_pin, 1);
    digitalWrite(motor1_backward_pin, 0);
  }

  void reverse() {
    digitalWrite(motor1_forward_pin, 0);
    digitalWrite(motor1_backward_pin, 1);
  }

  void stop() {
    digitalWrite(motor1_forward_pin, 0);
    digitalWrite(motor1_backward_pin, 0);
  }
};