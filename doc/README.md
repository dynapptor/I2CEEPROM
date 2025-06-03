# I2CEEPROM Library

## Overview
/**
 * @brief Library for synchronous I2C EEPROM operations with AT24LC chips.
 * @details Provides a StorageBase-derived interface for 4 KB to 64 KB EEPROMs,
 *          optimized for shutdown-time writes in home automation systems.
 */

The `I2CEEPROM` library enables reliable interaction with AT24LC series I2C EEPROM chips (e.g., AT24LC256, AT24LC512) for configuration and state storage. It supports synchronous read/write operations, with write verification for data integrity, and is designed for shutdown-time writes to minimize critical timing issues. The library is compatible with ESP32 and AVR platforms, using platform-specific buffer sizes.

## Features
- Synchronous API for simple, blocking operations.
- Write verification for all write methods (`writeu8`, `writeArray`, `updateArray`).
- Optimized block writes for shutdown-time configuration saving.
- Platform support: ESP32 (128-byte buffer), AVR (30-byte buffer).
- Error logging via `Logger.h` for I2C issues (e.g., NACK, timeout).
- Write cycle optimization with `updateu8` and `updateArray`.
- Optional hardware write protection via a pin.

## Installation
/**
 * @brief Instructions for installing the I2CEEPROM library.
 */
1. Clone or download the library:
   ```bash
   git clone https://github.com/your-repo/I2CEEPROM.git
   ```
2. Copy the `I2CEEPROM` folder to your Arduino `libraries` directory.
3. Include the library in your sketch:
   ```cpp
   #include <I2CEEPROM.h>
   ```

## Dependencies
/**
 * @brief Required dependencies for the library.
 */
- **Arduino Wire Library**: For I2C communication.
- **StorageBase**: Base class for storage operations.
- **Logger.h**: For error logging (optional, customizable).

## Usage
/**
 * @brief Example usage of the I2CEEPROM library.
 */

### Initialization
```cpp
#include <Wire.h>
#include <I2CEEPROM.h>

I2CEEPROM eeprom(0x50, AT24LC::AT24LC256, &Wire); // 32 KB EEPROM at 0x50

void setup() {
  Wire.begin();
  if (!eeprom.isConnected()) {
    Serial.println("EEPROM not found!");
    while (1);
  }
}
```

### Reading Configuration at Startup
```cpp
uint8_t config[100];
eeprom.readArray(0, config, 100); // Read 100 bytes from address 0
```

### Writing Configuration Before Shutdown
```cpp
uint8_t config[100] = {0x55}; // Example config data
if (eeprom.writeArray(0, config, 100)) {
  Serial.println("Config saved successfully");
} else {
  Serial.println("Config save failed");
}
```

### Updating Configuration (Web Interface)
```cpp
uint8_t newConfig[50] = {0xAA};
eeprom.updateArray(100, newConfig, 50); // Update only changed bytes
```

### Formatting EEPROM
```cpp
eeprom.format(0x00); // Fill EEPROM with zeros
```

## Configuration
/**
 * @brief Configuration options for the I2CEEPROM library.
 */
- **I2C Address**: Typically 0x50–0x57, set via constructor.
- **Chip Type**: Use `AT24LC` enum (e.g., `AT24LC256` for 32 KB).
- **Write Protect Pin**: Optional, set to -1 if not used.
- **Write Cycle Time**: Default 5 ms (`I2C_WRITEDELAY`), adjustable via `setExtraWriteCycleTime`.

## Notes
/**
 * @brief Additional information and considerations.
 */
- **Address Validation**: Prevents out-of-bounds access with `isValidAddress`.
- **Performance**: Block writes optimize shutdown operations (~10–15 ms for 100 bytes).
- **EEPROM Lifespan**: `updateu8`/`updateArray` minimize write cycles (100,000 cycles/cell).
- **Platform Considerations**:
  - **ESP32**: Supports FreeRTOS multitasking, larger buffer.
  - **AVR**: Suitable for constrained environments, smaller buffer.
- **Error Handling**: I2C errors logged for debugging.

## License
/**
 * @brief License information.
 */
MIT License. See `LICENSE` file for details.
