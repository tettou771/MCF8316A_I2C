#include "MCF8316A_I2C.h"

MCF8316A_I2C::MCF8316A_I2C() {
    _i2cAddress = MCF8316A_DEFAULT_ADDRESS;
    _crcEnabled = false;
    _debugEnabled = false;
    _debugStream = nullptr;
}

bool MCF8316A_I2C::begin(uint8_t address, TwoWire &wire) {
    _wire = &wire;
    _i2cAddress = address;
    
    _wire->begin();
    
    // Test communication by reading device ID
    uint16_t deviceId = readRegister16(MCF8316A_DEVICE_ID);
    
    if (_debugEnabled) {
        debugPrintHex("Device ID: 0x", deviceId);
    }
    
    // MCF8316A device ID should be non-zero
    return (deviceId != 0 && deviceId != 0xFFFF);
}

void MCF8316A_I2C::setI2CAddress(uint8_t address) {
    _i2cAddress = address;
}

uint32_t MCF8316A_I2C::readRegister32(uint32_t registerAddress) {
    uint32_t controlWord = createControlWord(true, _crcEnabled, 1, registerAddress);
    
    if (_debugEnabled) {
        debugPrintHex("Read32 Control Word: 0x", controlWord);
    }
    
    if (!sendControlWord(controlWord)) {
        return 0;
    }
    
    uint8_t data[5]; // 4 bytes data + optional CRC
    uint8_t readLength = _crcEnabled ? 5 : 4;
    
    if (!readData(data, readLength)) {
        return 0;
    }
    
    if (_crcEnabled) {
        uint8_t crc = calculateCRC8(data, 4);
        if (crc != data[4]) {
            debugPrint("CRC mismatch!");
            return 0;
        }
    }
    
    // LSB first
    uint32_t result = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    
    if (_debugEnabled) {
        debugPrintHex("Read32 Result: 0x", result);
    }
    
    return result;
}

bool MCF8316A_I2C::writeRegister32(uint32_t registerAddress, uint32_t data) {
    uint32_t controlWord = createControlWord(false, _crcEnabled, 1, registerAddress);
    
    if (_debugEnabled) {
        debugPrintHex("Write32 Control Word: 0x", controlWord);
        debugPrintHex("Write32 Data: 0x", data);
    }
    
    if (!sendControlWord(controlWord)) {
        return false;
    }
    
    uint8_t dataBytes[5];
    // LSB first
    dataBytes[0] = data & 0xFF;
    dataBytes[1] = (data >> 8) & 0xFF;
    dataBytes[2] = (data >> 16) & 0xFF;
    dataBytes[3] = (data >> 24) & 0xFF;
    
    uint8_t writeLength = 4;
    if (_crcEnabled) {
        dataBytes[4] = calculateCRC8(dataBytes, 4);
        writeLength = 5;
    }
    
    return writeData(dataBytes, writeLength);
}

uint16_t MCF8316A_I2C::readRegister16(uint32_t registerAddress) {
    uint32_t controlWord = createControlWord(true, _crcEnabled, 0, registerAddress);
    
    if (_debugEnabled) {
        debugPrintHex("Read16 Control Word: 0x", controlWord);
    }
    
    if (!sendControlWord(controlWord)) {
        return 0;
    }
    
    uint8_t data[3]; // 2 bytes data + optional CRC
    uint8_t readLength = _crcEnabled ? 3 : 2;
    
    if (!readData(data, readLength)) {
        return 0;
    }
    
    if (_crcEnabled) {
        uint8_t crc = calculateCRC8(data, 2);
        if (crc != data[2]) {
            debugPrint("CRC mismatch!");
            return 0;
        }
    }
    
    // LSB first
    uint16_t result = data[0] | (data[1] << 8);
    
    if (_debugEnabled) {
        debugPrintHex("Read16 Result: 0x", result);
    }
    
    return result;
}

bool MCF8316A_I2C::writeRegister16(uint32_t registerAddress, uint16_t data) {
    uint32_t controlWord = createControlWord(false, _crcEnabled, 0, registerAddress);
    
    if (_debugEnabled) {
        debugPrintHex("Write16 Control Word: 0x", controlWord);
        debugPrintHex("Write16 Data: 0x", data);
    }
    
    if (!sendControlWord(controlWord)) {
        return false;
    }
    
    uint8_t dataBytes[3];
    // LSB first
    dataBytes[0] = data & 0xFF;
    dataBytes[1] = (data >> 8) & 0xFF;
    
    uint8_t writeLength = 2;
    if (_crcEnabled) {
        dataBytes[2] = calculateCRC8(dataBytes, 2);
        writeLength = 3;
    }
    
    return writeData(dataBytes, writeLength);
}

void MCF8316A_I2C::enableCRC() {
    _crcEnabled = true;
}

void MCF8316A_I2C::disableCRC() {
    _crcEnabled = false;
}

bool MCF8316A_I2C::start() {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    devCtrl |= MCF8316A_CTRL_RUN_MOTOR;
    return writeRegister16(MCF8316A_DEV_CTRL, devCtrl);
}

bool MCF8316A_I2C::stop() {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    devCtrl &= ~MCF8316A_CTRL_RUN_MOTOR;
    return writeRegister16(MCF8316A_DEV_CTRL, devCtrl);
}

bool MCF8316A_I2C::coast() {
    // Coast is same as stop for this device
    return stop();
}

bool MCF8316A_I2C::brake() {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    devCtrl |= MCF8316A_CTRL_BRAKE;
    return writeRegister16(MCF8316A_DEV_CTRL, devCtrl);
}

bool MCF8316A_I2C::setSpeed(float rpm) {
    // Convert RPM to Hz (electrical frequency)
    // This depends on motor pole pairs, assuming 2 pole pairs
    float hz = (rpm / 60.0) * 2.0;
    
    // Speed is set as a 16-bit value in 0.1 Hz units
    uint16_t speedValue = (uint16_t)(hz * 10.0);
    
    return writeRegister16(MCF8316A_SPEED_CONTROL, speedValue);
}

float MCF8316A_I2C::getSpeed() {
    uint16_t speedHz = readRegister16(MCF8316A_MOTOR_SPEED);
    // Convert from 0.1 Hz units to RPM (assuming 2 pole pairs)
    float hz = speedHz / 10.0;
    return (hz * 60.0) / 2.0;
}

bool MCF8316A_I2C::setSpeedRange(uint8_t range) {
    uint16_t config = readRegister16(MCF8316A_CLOSED_LOOP1);
    config &= ~(0x03 << 14); // Clear speed range bits
    config |= ((range & 0x03) << 14);
    return writeRegister16(MCF8316A_CLOSED_LOOP1, config);
}

bool MCF8316A_I2C::setDirection(bool forward) {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    if (forward) {
        devCtrl &= ~MCF8316A_CTRL_DIRECTION;
    } else {
        devCtrl |= MCF8316A_CTRL_DIRECTION;
    }
    return writeRegister16(MCF8316A_DEV_CTRL, devCtrl);
}

bool MCF8316A_I2C::getDirection() {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    return !(devCtrl & MCF8316A_CTRL_DIRECTION);
}

uint16_t MCF8316A_I2C::getFaultStatus() {
    uint16_t gateFaults = getGateFaultStatus();
    uint16_t controllerFaults = getControllerFaultStatus();
    return gateFaults | controllerFaults;
}

uint16_t MCF8316A_I2C::getGateFaultStatus() {
    return readRegister16(MCF8316A_GATE_DRIVER_FAULT_STATUS);
}

uint16_t MCF8316A_I2C::getControllerFaultStatus() {
    return readRegister16(MCF8316A_CONTROLLER_FAULT_STATUS);
}

uint8_t MCF8316A_I2C::getAlgorithmState() {
    uint16_t status = readRegister16(MCF8316A_ALGO_STATUS);
    return (status >> 13) & 0x07; // Bits 15:13
}

bool MCF8316A_I2C::clearFaults() {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    devCtrl |= MCF8316A_CTRL_CLR_FAULT;
    return writeRegister16(MCF8316A_DEV_CTRL, devCtrl);
}

bool MCF8316A_I2C::isFault() {
    return getFaultStatus() != 0;
}

bool MCF8316A_I2C::isRunning() {
    uint8_t state = getAlgorithmState();
    return (state == MCF8316A_ALGO_STATE_STARTUP || 
            state == MCF8316A_ALGO_STATE_CLOSED_LOOP);
}

bool MCF8316A_I2C::saveToEEPROM() {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    devCtrl |= MCF8316A_CTRL_SAVE_TO_EEPROM;
    
    if (!writeRegister16(MCF8316A_DEV_CTRL, devCtrl)) {
        return false;
    }
    
    return waitForEEPROMDone();
}

bool MCF8316A_I2C::loadFromEEPROM() {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    devCtrl |= MCF8316A_CTRL_LOAD_FROM_EEPROM;
    
    if (!writeRegister16(MCF8316A_DEV_CTRL, devCtrl)) {
        return false;
    }
    
    return waitForEEPROMDone();
}

bool MCF8316A_I2C::waitForEEPROMDone(uint32_t timeout) {
    uint32_t startTime = millis();
    
    while ((millis() - startTime) < timeout) {
        uint16_t status = readRegister16(MCF8316A_EEPROM_WR_DONE);
        if (status & 0x01) {
            return true;
        }
        delay(10);
    }
    
    return false;
}

bool MCF8316A_I2C::setMotorParameters(uint16_t resistance_mohm, uint16_t inductance_uh) {
    // Motor resistance in 1 mOhm units (bits 15:8)
    // Motor inductance in 1 uH units (bits 7:0)
    uint16_t params = ((resistance_mohm & 0xFF) << 8) | (inductance_uh & 0xFF);
    return writeRegister16(MCF8316A_MTR_PARAMS, params);
}

void MCF8316A_I2C::getMotorParameters(uint16_t &resistance_mohm, uint16_t &inductance_uh) {
    uint16_t params = readRegister16(MCF8316A_MTR_PARAMS);
    resistance_mohm = (params >> 8) & 0xFF;
    inductance_uh = params & 0xFF;
}

bool MCF8316A_I2C::enableSpeedLoop() {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    devCtrl |= MCF8316A_CTRL_SPEED_MODE;
    return writeRegister16(MCF8316A_DEV_CTRL, devCtrl);
}

bool MCF8316A_I2C::disableSpeedLoop() {
    uint16_t devCtrl = readRegister16(MCF8316A_DEV_CTRL);
    devCtrl &= ~MCF8316A_CTRL_SPEED_MODE;
    return writeRegister16(MCF8316A_DEV_CTRL, devCtrl);
}

bool MCF8316A_I2C::setSpeedProfile(uint8_t profile, uint16_t speed_hz, uint16_t accel_hz_s) {
    if (profile > 4) return false;
    
    uint32_t regAddr = MCF8316A_SPEED_PROFILES1 + (profile * 2);
    uint16_t profileData = ((accel_hz_s & 0xFF) << 8) | (speed_hz & 0xFF);
    
    return writeRegister16(regAddr, profileData);
}

float MCF8316A_I2C::getTemperature() {
    // Temperature reading would require specific register implementation
    // This is a placeholder
    return 25.0;
}

float MCF8316A_I2C::getVoltage() {
    // Read DC bus voltage
    uint16_t voltage = readRegister16(MCF8316A_DC_BUS_CURRENT) >> 8;
    return voltage * 0.1; // Convert to volts
}

float MCF8316A_I2C::getCurrent() {
    // Read DC bus current
    uint16_t current = readRegister16(MCF8316A_DC_BUS_CURRENT) & 0xFF;
    return current * 0.01; // Convert to amps
}

void MCF8316A_I2C::enableDebug(Stream &debugStream) {
    _debugEnabled = true;
    _debugStream = &debugStream;
}

void MCF8316A_I2C::disableDebug() {
    _debugEnabled = false;
    _debugStream = nullptr;
}

// Private methods

bool MCF8316A_I2C::sendControlWord(uint32_t controlWord) {
    _wire->beginTransmission(_i2cAddress);
    
    // Send control word MSB first (24 bits = 3 bytes)
    _wire->write((controlWord >> 16) & 0xFF);
    delayMicroseconds100();
    _wire->write((controlWord >> 8) & 0xFF);
    delayMicroseconds100();
    _wire->write(controlWord & 0xFF);
    
    return (_wire->endTransmission() == 0);
}

bool MCF8316A_I2C::writeData(uint8_t *data, uint8_t length) {
    _wire->beginTransmission(_i2cAddress);
    
    for (uint8_t i = 0; i < length; i++) {
        _wire->write(data[i]);
        if (i < length - 1) {
            delayMicroseconds100();
        }
    }
    
    return (_wire->endTransmission() == 0);
}

bool MCF8316A_I2C::readData(uint8_t *data, uint8_t length) {
    uint8_t received = _wire->requestFrom(_i2cAddress, length);
    
    if (received != length) {
        return false;
    }
    
    for (uint8_t i = 0; i < length; i++) {
        data[i] = _wire->read();
        if (i < length - 1) {
            delayMicroseconds100();
        }
    }
    
    return true;
}

uint8_t MCF8316A_I2C::calculateCRC8(uint8_t *data, uint8_t length) {
    uint8_t crc = MCF8316A_CRC8_INIT;
    
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ MCF8316A_CRC8_POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}

void MCF8316A_I2C::delayMicroseconds100() {
    delayMicroseconds(100);
}

uint32_t MCF8316A_I2C::createControlWord(bool isRead, bool crcEnable, uint8_t dataLength, uint32_t address) {
    uint32_t controlWord = 0;
    
    if (isRead) {
        controlWord |= MCF8316A_RW_READ;
    }
    
    if (crcEnable) {
        controlWord |= MCF8316A_CRC_ENABLE;
    }
    
    // Data length: 0 = 16-bit, 1 = 32-bit, 2 = 64-bit
    controlWord |= ((dataLength & 0x03) << 20);
    
    // Address (20 bits)
    controlWord |= (address & 0xFFFFF);
    
    return controlWord;
}

void MCF8316A_I2C::debugPrint(const char *message) {
    if (_debugEnabled && _debugStream) {
        _debugStream->println(message);
    }
}

void MCF8316A_I2C::debugPrintHex(const char *prefix, uint32_t value) {
    if (_debugEnabled && _debugStream) {
        _debugStream->print(prefix);
        _debugStream->println(value, HEX);
    }
}