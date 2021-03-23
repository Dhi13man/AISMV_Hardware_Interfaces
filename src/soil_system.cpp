#include <Arduino.h>
#include <EEPROM.h>
#include <ESP32_Servo.h>

#include "motor_drivers.cpp"

/// Responsible for
///     1. Soil moisture sensor handling and reading using Servo
///     2. Watering the soil
class SoilInterface {
    private:
    /// Controlls the motor driver that runs the pump
    WaterPumpMotorDriverInterface l298Controller;

    /// Servo responsible 
    Servo moistureArmServo;

    /// Analog reading from Moisture sensor through this pin
    int sensorPin, analogMoistureValue;

    /// Target moisture that plant requires, in milliLiter.
    ///
    /// Provided by user using mobile app and stored persistently in EEPROM.
    float targetMoisturePercentage;
    
    /// Moisture of the soil that is increased when 1 mL of water is added to the soil.
    ///
    /// Provided by user using mobile app and stored persistently in EEPROM.
    float moisturePerMilliliter;

    /// Direction of the farm that AISMV will start in
    ///
    /// Provided by user using mobile app and stored persistently in EEPROM.
    char farmDirection;

    /// State of The Soil Interface System
    String systemState;

    void raiseArm() {
        systemState = "Raising Arm";
        moistureArmServo.write(180);
    }

    void lowerArm() {
        systemState = "Lowering Arm";
        moistureArmServo.write(165);
    }

    /// Gets soil Moisture percentage only once sensor is inserted through Servo. Private member.
    float getMoisturePercentage() {
        analogMoistureValue = analogRead(sensorPin);
        delay(500);
        return map(analogMoistureValue, 1450, 4095, 100, 0);
    }

    public:
    /// Initializes the Water pump Motor Driver, Servo and its pins.
    SoilInterface(int moistureSensorPin=15, int armServoPin=2, int waterPumpMotorPin=12) {
        l298Controller = WaterPumpMotorDriverInterface(waterPumpMotorPin);   // Pump
        // Moisture Sensor Arm
        pinMode(armServoPin, OUTPUT);
        moistureArmServo = Servo();
        moistureArmServo.attach(armServoPin);
        moistureArmServo.write(180);
        // Moisture Sensor
        pinMode(moistureSensorPin, INPUT);
        sensorPin = moistureSensorPin;
        // Soil Parameters Extraction
        // initialize EEPROM with predefined size
        EEPROM.begin(100);
        //  For Target Moisture
        EEPROM.get<float>(1, targetMoisturePercentage);
        if (isnan(moisturePerMilliliter) || targetMoisturePercentage == NAN || farmDirection < 0) {      // Initial Null check
            targetMoisturePercentage = -1;
            EEPROM.put<float>(1, targetMoisturePercentage);
            EEPROM.commit();
        }
        //  For Moisture per Milliliter
        EEPROM.get<float>(11, moisturePerMilliliter);
        if (isnan(moisturePerMilliliter) || moisturePerMilliliter == NAN || farmDirection < 0) {         // Initial Null Check
            moisturePerMilliliter = -1;
            EEPROM.put<float>(11, moisturePerMilliliter);
            EEPROM.commit();
        }
        //  For Moisture per Milliliter
        EEPROM.get<char>(21, farmDirection);
        if (isnan(farmDirection) || farmDirection == NAN || (farmDirection != 'l' && farmDirection != 'r')) {         // Initial Null Check
            farmDirection = 'l';
            EEPROM.put<char>(21, farmDirection);
            EEPROM.commit();
        }
        systemState = "Initialized";
    }

    /// Get chosen set Soil Parameter.
    ///
    /// [getParameterType] == 't' -> Currently set [targetMoisturePercentage] is returned
    /// [getParameterType] == 'm' -> Currently set [moisturePerMilliliter] is returned
    float getSoilParameter(char getParameterType='t') {
        return (getParameterType == 'm') ? moisturePerMilliliter : targetMoisturePercentage;
    }

    /// Get farm direction set in AISMV
    char getFarmDirection() {
        return farmDirection;
    }

    /// Update chosen Soil Parameter value to [updatedValue]
    ///
    /// [updateType] == 't' -> [targetMoisturePercentage] is updated
    /// [updateType] == 'm' -> [moisturePerMilliliter] is updated
    int updateSoilParameters(float updatedValue, char updateType='t') {
        if (updateType == 't') {
            targetMoisturePercentage = updatedValue;
            EEPROM.put<float>(1, targetMoisturePercentage);
            EEPROM.commit();
            return 0;
        }
        if (updateType == 'm') {
            moisturePerMilliliter = updatedValue;
            EEPROM.put<float>(11, moisturePerMilliliter);
            EEPROM.commit();
            return 0;
        }
        return 1;
    }

    /// Update farm direction set in AISMV
    int updateFarmDirection(char updatedValue) {
        farmDirection = updatedValue;
        EEPROM.put<char>(21, farmDirection);
        EEPROM.commit();
        return 1;
    }

    /// Insert Soil Moisture Sensor -> Calculate Moisture Percentage -> Remove Moisture Sensor -> Water Soil -> Move on
    float detectMoistureThenWater() {
        lowerArm();
        float moisturePercentage = getMoisturePercentage();
        delay(1500);
        raiseArm();
        waterSoil();
        return moisturePercentage;
    }

    /// Insert Soil Moisture Sensor -> Calculate Moisture Percentage -> Water Soil 
    /// -> Calculate Moisture Percentage ->  Water Soil -> Calculate Moisture Percentage ->  Water Soil...
    /// Until Target moisture level reached.
    /// -> Finally, Remove Moisture Sensor -> Move on
    float detectMoistureAndWaterUntilTargetMoisture() {
        lowerArm();
        while (getMoisturePercentage() < targetMoisturePercentage)
            waterSoil();
        raiseArm();
        return getMoisturePercentage();
    }

    /// Waters the soil for appropriate time for optimum irrigation. 
    ///
    /// Turns on Electric Water pump for [waterTimeMillisecond] time then stops it.
    void waterSoil(int waterTimeMilliseconds=1000) {
        l298Controller.forward();
        delay(waterTimeMilliseconds);
        l298Controller.stop();
    }

    ~SoilInterface() {
        moistureArmServo.detach();
    }
};