#ifndef MCF8316A_I2C_H
#define MCF8316A_I2C_H

#include <Arduino.h>
#include <Wire.h>
#include "MCF8316A_registers.h"

#define MCF8316A_DEFAULT_ADDRESS 0x01

class MCF8316A_I2C {
public:
    // Constructor
    MCF8316A_I2C();
    
    // Initialization
    bool begin(uint8_t address = MCF8316A_DEFAULT_ADDRESS, TwoWire &wire = Wire);
    void setI2CAddress(uint8_t address);
    
    // Register Access
    uint32_t readRegister32(uint32_t registerAddress);
    bool writeRegister32(uint32_t registerAddress, uint32_t data);
    uint16_t readRegister16(uint32_t registerAddress);
    bool writeRegister16(uint32_t registerAddress, uint16_t data);
    
    // CRC Configuration
    void enableCRC();
    void disableCRC();
    
    // Motor Control
    bool start();
    bool stop();
    bool coast();
    bool brake();
    
    // Speed Control
    bool setSpeed(float rpm);
    float getSpeed();
    bool setSpeedRange(uint8_t range);
    
    // Direction Control
    bool setDirection(bool forward);
    bool getDirection();
    
    // Status and Faults
    uint16_t getFaultStatus();
    uint16_t getGateFaultStatus();
    uint16_t getControllerFaultStatus();
    uint8_t getAlgorithmState();
    bool clearFaults();
    bool isFault();
    bool isRunning();
    
    // EEPROM Operations
    bool saveToEEPROM();
    bool loadFromEEPROM();
    bool waitForEEPROMDone(uint32_t timeout = 5000);
    
    // Motor Parameters
    bool setMotorParameters(uint16_t resistance_mohm, uint16_t inductance_uh);
    void getMotorParameters(uint16_t &resistance_mohm, uint16_t &inductance_uh);
    
    // Speed Loop Control
    bool enableSpeedLoop();
    bool disableSpeedLoop();
    
    // Speed Profiles
    bool setSpeedProfile(uint8_t profile, uint16_t speed_hz, uint16_t accel_hz_s);
    
    // Monitoring
    float getTemperature();
    float getVoltage();
    float getCurrent();
    
    // Debug
    void enableDebug(Stream &debugStream);
    void disableDebug();
    
private:
    TwoWire *_wire;
    uint8_t _i2cAddress;
    bool _crcEnabled;
    bool _debugEnabled;
    Stream *_debugStream;
    
    // I2C Communication Helpers
    bool sendControlWord(uint32_t controlWord);
    bool writeData(uint8_t *data, uint8_t length);
    bool readData(uint8_t *data, uint8_t length);
    uint8_t calculateCRC8(uint8_t *data, uint8_t length);
    void delayMicroseconds100();
    
    // Control Word Generation
    uint32_t createControlWord(bool isRead, bool crcEnable, uint8_t dataLength, uint32_t address);
    
    // Debug Output
    void debugPrint(const char *message);
    void debugPrintHex(const char *prefix, uint32_t value);
};

#endif // MCF8316A_I2C_H