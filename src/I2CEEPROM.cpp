#include "I2CEEPROM.h"

#include <Wire.h>

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
#define I2C_BUFFERSIZE 128
#else
#define I2C_BUFFERSIZE 30
#endif

I2CEEPROM::I2CEEPROM(uint8_t i2cAddress, AT24LC ATDEV, TwoWire* wire, int8_t writeProtectPin) {
  _i2cAddress = i2cAddress;
  _deviceSize = static_cast<uint32_t>(ATDEV);
  _pageSize = getPageSize(ATDEV);
  _wire = wire;
  _writeProtectPin = writeProtectPin;
  if (_writeProtectPin > -1) {
    pinMode(_writeProtectPin, OUTPUT);
    digitalWrite(_writeProtectPin, HIGH);
  }
}

I2CEEPROM::~I2CEEPROM() {}

uint8_t I2CEEPROM::getPageSize(AT24LC ATDEV) {
  if (static_cast<uint32_t>(ATDEV) <= 2048) return 16;
  if (static_cast<uint32_t>(ATDEV) <= 8192) return 32;
  if (static_cast<uint32_t>(ATDEV) <= 32768) return 64;
  return 128;
}

uint8_t I2CEEPROM::getPageSize() { return _pageSize; }

uint32_t I2CEEPROM::getLastWrite() { return _lastWrite; }

void I2CEEPROM::setExtraWriteCycleTime(uint8_t ms) { _extraTWR = ms; }

uint8_t I2CEEPROM::getExtraWriteCycleTime() { return _extraTWR; }

bool I2CEEPROM::isConnected() {
  _wire->beginTransmission(_i2cAddress);
  return (_wire->endTransmission() == 0);
}

uint8_t I2CEEPROM::getAddress() {
  return _i2cAddress;
}

uint8_t I2CEEPROM::readu8(uint16_t addr) {
  if (!isValidAddress(addr)) return 0;
  uint8_t rdata;
  _ReadBlock(addr, &rdata, 1);
  return rdata;
}

bool I2CEEPROM::writeu8(uint16_t addr, uint8_t data) {
  if (!isValidAddress(addr)) return false;
  _WriteBlock(addr, &data, 1);
  return _verifyBlock(addr, &data, 1);
}

bool I2CEEPROM::updateu8(uint16_t addr, uint8_t val) {
  if (!isValidAddress(addr)) return false;
  if (val == readu8(addr)) return 1;
  return writeu8(addr, val);
}

bool I2CEEPROM::format(uint8_t v) {
  uint8_t buffer[I2C_BUFFERSIZE]{v};
  return _writeArray(0, buffer, _deviceSize, false);
}

uint32_t I2CEEPROM::getSize() { return _deviceSize; }

uint8_t* I2CEEPROM::readArray(uint16_t addr, uint8_t* buffer, size_t length) {
  if (!isValidAddress(addr, length)) return 0;
  uint16_t address = addr;
  uint16_t len = length;
  uint16_t bytes = 0;
  while (len > 0) {
    uint8_t count = I2C_BUFFERSIZE;
    if (count > len) count = len;
    bytes += _ReadBlock(address, buffer, count);
    address += count;
    buffer += count;
    len -= count;
  }
  return buffer;
}

bool I2CEEPROM::writeArray(uint16_t addr, const uint8_t* buffer, size_t length) {
  if (!isValidAddress(addr, length)) return 0;
  _writeArray(addr, buffer, length, true);
  return verifyArray(addr, buffer, length);
}

bool I2CEEPROM::updateArray(uint16_t addr, const uint8_t* buffer, size_t length) {
  if (!isValidAddress(addr, length)) return 0;
  uint16_t address = addr;
  uint16_t len = length;
  uint16_t bytes = 0;
  while (len > 0) {
    uint8_t buf[I2C_BUFFERSIZE];
    uint8_t count = I2C_BUFFERSIZE;

    if (count > len) count = len;
    _ReadBlock(address, buf, count);
    if (memcmp(buffer, buf, count) != 0) {
      _writeArray(address, buffer, count, true);
      bytes += count;
    }
    address += count;
    buffer += count;
    len -= count;
  }
  return verifyArray(addr, buffer, len);
}

void I2CEEPROM::_beginTransmission(uint16_t addr) {
  if (_writeProtectPin > -1) digitalWrite(_writeProtectPin, LOW);
  _wire->beginTransmission(_i2cAddress);
  _wire->write((addr >> 8));
  _wire->write((addr & 0xFF));
}

uint8_t I2CEEPROM::_endTransmission() {
  uint8_t ret = _wire->endTransmission();
  if (_writeProtectPin > -1) digitalWrite(_writeProtectPin, HIGH);
  if (ret != 0) {
    logger.error(F("I2C error: addr=%d, error=%d"), _i2cAddress, ret);
  }
  return ret;
}

void I2CEEPROM::_waitATReady() {
  uint32_t waitTime = I2C_WRITEDELAY + _extraTWR * 1000UL;
  while ((micros() - _lastWrite) <= waitTime) {
    if (isConnected()) return;
    yield();
  }
  return;
}

uint8_t I2CEEPROM::_WriteBlock(uint16_t addr, const uint8_t* buffer, uint16_t length) {
  _waitATReady();
  _beginTransmission(addr);
  _wire->write(buffer, length);
  uint8_t ret = _endTransmission();
  _lastWrite = micros();
  yield();
  return ret;
}

uint8_t I2CEEPROM::_ReadBlock(uint16_t addr, uint8_t* buffer, uint16_t length) {
  _waitATReady();

  _beginTransmission(addr);
  if (_endTransmission() != 0) return 0;

  uint8_t readBytes = 0;
  readBytes = _wire->requestFrom((int)_i2cAddress, (int)length);
  yield();
  if (readBytes != length) {
    logger.error(F("Read mismatch: addr=%d, requested=%d, received=%d"), addr, length, readBytes);
  }
  uint8_t count = 0;
  while (count < readBytes) {
    buffer[count++] = _wire->read();
  }
  return readBytes;
}

bool I2CEEPROM::_verifyBlock(uint16_t addr, const uint8_t* buffer, uint16_t length) {
  _waitATReady();

  _beginTransmission(addr);
  if (_endTransmission() != 0) return false;

  uint8_t readBytes = 0;
  readBytes = _wire->requestFrom((int)_i2cAddress, (int)length);
  yield();
  uint8_t count = 0;
  while (count < readBytes) {
    if (buffer[count++] != _wire->read()) {
      return false;
    }
  }
  return true;
}

bool I2CEEPROM::_writeArray(uint16_t addr, const uint8_t* buffer, uint16_t length, bool incrBuffer) {
  uint16_t address = addr;
  uint16_t len = length;
  while (len > 0) {
    uint8_t bytesUntilPageBoundary = _pageSize - address % _pageSize;

    uint8_t count = I2C_BUFFERSIZE;
    if (count > len) count = len;
    if (count > bytesUntilPageBoundary) count = bytesUntilPageBoundary;

    if (_WriteBlock(address, buffer, count) != 0) return 0;

    address += count;
    if (incrBuffer) buffer += count;
    len -= count;
  }
  return 1;
}

bool I2CEEPROM::verifyArray(uint16_t addr, const uint8_t* buffer, uint16_t length) {
  uint16_t address = addr;
  uint16_t len = length;
  while (len > 0) {
    uint8_t count = I2C_BUFFERSIZE;
    if (count > len) count = len;
    if (_verifyBlock(address, buffer, count) == false) {
      return false;
    }
    address += count;
    buffer += count;
    len -= count;
  }
  return true;
}

void I2CEEPROM::flush() {}