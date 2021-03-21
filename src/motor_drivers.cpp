#include <Arduino.h>

/// Controlling interface class for Sabertooth motor driver through SERIAL communication.
class SabertoothInterface {
  private:
  String state;

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
    state = "Initialized";
    Serial2.begin(9600, SERIAL_8N1);
  }

  void smoothRight(int speed=400) {
    moveMotor(1, speed);
    moveMotor(2, 0);
    state = "Moving Right Smoothly";
  }

  void smoothLeft(int speed=400) {
    moveMotor(2, -speed);
    moveMotor(1, 0);
    state = "Moving Left Smoothly";
  }

  void hardRight(int speed=400) {
    moveMotor(1, speed);
    moveMotor(2, speed);
    state = "Moving Hard Right";
  }

  void hardLeft(int speed=400) {
    moveMotor(2, -speed);
    moveMotor(1, -speed);
    state = "Moving Hard Left";
  }

  void forward(int speed=400) {
    moveMotor(1, speed);
    moveMotor(2, -speed);
    state = "Moving Forward";
  }

  void reverse(int speed=400) {
    moveMotor(1, -speed);
    moveMotor(2, speed);
    state = "Moving Reverse";
  }

  void stop() {
    moveMotor(1, 0);
    moveMotor(2, 0);
    state = "Stopped";
  }

  String getState() {
    return state;
  }

  ~SabertoothInterface() {
    stop();
    Serial2.end();
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