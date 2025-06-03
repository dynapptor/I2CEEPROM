# I2CEEPROM API Documentation

## Overview
The `I2CEEPROM` class provides a synchronous interface for managing AT24LC series I2C EEPROM chips (4 KB to 64 KB, e.g., AT24LC256, AT24LC512) in home automation systems. It inherits from `StorageBase` and is optimized for reliable data storage, particularly for shutdown-time writes to ensure data integrity. Write operations include verification, and the library supports ESP32 and AVR platforms with platform-specific buffer sizes (128 bytes for ESP32, 30 bytes for AVR).

## Class Declaration
```cpp
/**
 * @brief I2CEEPROM class for synchronous I2C EEPROM operations.
 * @details Inherits from StorageBase, supports AT24LC chips (4 KB to 64 KB),
 *          optimized for shutdown-time writes with verification.
 */
class I2CEEPROM : public StorageBase {
 public:
  I2CEEPROM(uint8_t i2cAddress, AT24LC ATDEV, TwoWire* wire, int8_t writeProtectPin = -1);
  ~I2CEEPROM();
  bool isConnected();
  uint8_t getAddress();
  uint8_t readu8(uint16_t addr);
  bool writeu8(uint16_t addr, uint8_t val);
  bool updateu8(uint16_t addr, uint8_t val);
  bool format(uint8_t v);
  uint16_t getSize();
  uint8_t* readArray(uint16_t addr, uint8_t* buffer, size_t length);
  bool writeArray(uint16_t addr, const uint8_t* buffer, size_t length);
  bool updateArray(uint16_t addr, const uint8_t* buffer, size_t length);
  void flush();
  bool verifyArray(uint16_t addr, const uint8_t* buffer, uint16_t length);
  uint8_t getPageSize();
  uint8_t getPageSize(AT24LC ATDEV);
  uint32_t getLastWrite();
  void setExtraWriteCycleTime(uint8_t ms);
  uint8_t getExtraWriteCycleTime();
};
```

## Public Methods

### Constructor
```cpp
/**
 * @brief Constructs an I2CEEPROM instance.
 * @param i2cAddress I2C address (e.g., 0x50).
 * @param ATDEV Chip type (e.g., AT24LC256).
 * @param wire TwoWire object for I2C communication.
 * @param writeProtectPin Write protection pin (-1 if unused).
 */
I2CEEPROM(uint8_t i2cAddress, AT24LC ATDEV, TwoWire* wire, int8_t writeProtectPin = -1)
```
- **Example**:
  ```cpp
  I2CEEPROM eeprom(0x50, AT24LC::AT24LC256, &Wire); // 32 KB EEPROM
  ```

### Destructor
```cpp
/**
 * @brief Destructor.
 */
~I2CEEPROM()
```

### isConnected
```cpp
/**
 * @brief Checks if the EEPROM is connected.
 * @return true if connected, false otherwise.
 */
bool isConnected()
```
- **Example**:
  ```cpp
  if (eeprom.isConnected()) {
    Serial.println("EEPROM connected");
  }
  ```

### getAddress
```cpp
/**
 * @brief Returns the I2C address of the EEPROM.
 * @return I2C address (e.g., 0x50).
 */
uint8_t getAddress()
```
- **Example**:
  ```cpp
  uint8_t addr = eeprom.getAddress(); // Returns 0x50
  ```

### readu8
```cpp
/**
 * @brief Reads a single byte from the specified address.
 * @param addr Memory address (0 to device size).
 * @return The byte read, or 0 if address invalid.
 */
uint8_t readu8(uint16_t addr)
```
- **Example**:
  ```cpp
  uint8_t value = eeprom.readu8(100); // Read byte at address 100
  ```

### writeu8
```cpp
/**
 * @brief Writes a single byte to the specified address, with verification.
 * @param addr Memory address.
 * @param val Byte to write.
 * @return true if successful, false if failed or address invalid.
 */
bool writeu8(uint16_t addr, uint8_t val)
```
- **Example**:
  ```cpp
  if (eeprom.writeu8(100, 0x55)) {
    Serial.println("Write successful");
  }
  ```

### updateu8
```cpp
/**
 * @brief Updates a byte only if it differs from the current value.
 * @param addr Memory address.
 * @param val Byte to write.
 * @return true if successful or no write needed, false if failed.
 */
bool updateu8(uint16_t addr, uint8_t val)
```
- **Example**:
  ```cpp
  eeprom.updateu8(100, 0x55); // Updates only if different
  ```

### format
```cpp
/**
 * @brief Fills the entire EEPROM with the specified value.
 * @param v Byte value to fill.
 * @return true if successful, false if failed.
 */
bool format(uint8_t v)
```
- **Example**:
  ```cpp
  eeprom.format(0x00); // Fill with zeros
  ```

### getSize
```cpp
/**
 * @brief Returns the EEPROM size in bytes.
 * @return Size (e.g., 32768 for AT24LC256).
 */
uint16_t getSize()
```
- **Example**:
  ```cpp
  uint16_t size = eeprom.getSize(); // Returns 32768
  ```

### readArray
```cpp
/**
 * @brief Reads an array of bytes into the provided buffer.
 * @param addr Starting address.
 * @param buffer Buffer to store data.
 * @param length Number of bytes to read.
 * @return Pointer to the buffer, or nullptr if address invalid.
 */
uint8_t* readArray(uint16_t addr, uint8_t* buffer, size_t length)
```
- **Example**:
  ```cpp
  uint8_t data[100];
  eeprom.readArray(100, data, 100); // Read 100 bytes
  ```

### writeArray
```cpp
/**
 * @brief Writes an array of bytes, with verification.
 * @param addr Starting address.
 * @param buffer Data to write.
 * @param length Number of bytes to write.
 * @return true if successful, false if failed or address invalid.
 */
bool writeArray(uint16_t addr, const uint8_t* buffer, size_t length)
```
- **Example**:
  ```cpp
  uint8_t data[100] = {0x55};
  eeprom.writeArray(100, data, 100); // Write 100 bytes
  ```

### updateArray
```cpp
/**
 * @brief Updates an array only for differing bytes.
 * @param addr Starting address.
 * @param buffer Data to write.
 * @param length Number of bytes to update.
 * @return true if successful, false if failed or address invalid.
 */
bool updateArray(uint16_t addr, const uint8_t* buffer, size_t length)
```
- **Example**:
  ```cpp
  uint8_t data[100] = {0xAA};
  eeprom.updateArray(100, data, 100); // Update 100 bytes
  ```

### flush
```cpp
/**
 * @brief Empty flush method, as I2C EEPROM writes are immediate.
 */
void flush()
```
- **Example**:
  ```cpp
  eeprom.flush(); // No effect
  ```

### verifyArray
```cpp
/**
 * @brief Verifies that the array matches the buffer.
 * @param addr Starting address.
 * @param buffer Data to verify against.
 * @param length Number of bytes to verify.
 * @return true if verified, false if mismatch or address invalid.
 */
bool verifyArray(uint16_t addr, const uint8_t* buffer, uint16_t length)
```
- **Example**:
  ```cpp
  uint8_t data[100] = {0x55};
  if (eeprom.verifyArray(100, data, 100)) {
    Serial.println("Data verified");
  }
  ```

### getPageSize
```cpp
/**
 * @brief Returns the page size of the current EEPROM.
 * @return Page size in bytes (e.g., 64 for AT24LC256).
 */
uint8_t getPageSize()

/**
 * @brief Returns the page size for a specified chip type.
 * @param ATDEV Chip type.
 * @return Page size in bytes.
 */
uint8_t getPageSize(AT24LC ATDEV)
```
- **Example**:
  ```cpp
  uint8_t pageSize = eeprom.getPageSize(); // Returns 64
  ```

### getLastWrite
```cpp
/**
 * @brief Returns the timestamp of the last write operation.
 * @return Timestamp in microseconds.
 */
uint32_t getLastWrite()
```
- **Example**:
  ```cpp
  uint32_t lastWrite = eeprom.getLastWrite();
  ```

### setExtraWriteCycleTime
```cpp
/**
 * @brief Sets additional write cycle wait time.
 * @param ms Extra wait time in milliseconds.
 */
void setExtraWriteCycleTime(uint8_t ms)
```
- **Example**:
  ```cpp
  eeprom.setExtraWriteCycleTime(1); // Add 1 ms
  ```

### getExtraWriteCycleTime
```cpp
/**
 * @brief Returns the additional write cycle wait time.
 * @return Extra wait time in milliseconds.
 */
uint8_t getExtraWriteCycleTime()
```
- **Example**:
  ```cpp
  uint8_t extraTime = eeprom.getExtraWriteCycleTime(); // Returns 1
  ```

## Notes
- **Address Validation**: All public methods use `isValidAddress` to prevent out-of-bounds access.
- **Write Verification**: Ensures data integrity for shutdown-time writes.
- **Platform Support**: ESP32 (128-byte buffer) and AVR (30-byte buffer).
- **Logging**: I2C errors logged via `Logger.h`.