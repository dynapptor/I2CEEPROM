#pragma once
// Komment: Helyes, a header guard szerepét betölti, megakadályozza a többszörös includolást.
// Javaslat: Alternatívaként használhatsz #ifndef I2CEEPROM_H / #define I2CEEPROM_H / #endif szerkezetet, de a #pragma once modern és egyszerűbb.

#include <StorageBase.h>
// Komment: A StorageBase header includolása szükséges, mivel az I2CEEPROM osztály ebből származik.
// Ellenőrzés: Feltételezem, hogy a StorageBase.h tartalmazza az absztrakt metódusokat (readu8, writeu8, stb.) és az isValidAddress deklarációt. Helyes.

class TwoWire;
// Komment: Előre deklaráció a TwoWire osztályra, helyes, mivel csak pointerként használjuk (_wire), így nem kell includolni a <Wire.h>-t, csökkentve a függőségeket.
// Ellenőrzés: A TwoWire a Wire könyvtár része, és kompatibilis ESP32 és AVR környezetekkel. Nincs hiba.

enum class AT24LC : uint32_t {
  AT24LC32 = 4096,
  AT24LC64 = 8192,
  AT24LCC128 = 16384,
  AT24LCC256 = 32768,
  AT24LC512 = 65536
};
// Komment: Az AT24LC enum meghatározza a támogatott I2C EEPROM chipek méreteit bájtban (pl. AT24LC256 = 32 KB). A uint32_t típus megfelelő, mivel a méretek 4096–65536 bájt között vannak.
// Ellenőrzés: A méretek pontosak a Microchip AT24LC sorozathoz. Az AT24LCC128 és AT24LCC256 elnevezés szokatlan (általában AT24C128, AT24C256), de konzisztens a kóddal. Javaslat: Ellenőrizd, hogy a dupla C (AT24LCC) szándékos-e, vagy elírás (AT24LC128, AT24LC256 lenne a szabványos).
// Javaslat: Dokumentáld Doxygen kommenttel az enumot, pl.:
// /**
//  * @brief Supported AT24LC I2C EEPROM chip sizes in bytes.
//  */
#ifndef I2C_WRITEDELAY
#define I2C_WRITEDELAY 5000
#endif
// Komment: Az I2C_WRITEDELAY definiálja az írási ciklus (TWB) alapértelmezett várakozási idejét (5000 µs = 5 ms). Ez illik az AT24LC chipek specifikációjához (általában 4-5 ms).
// Ellenőrzés: A 5000 µs konzervatív, de biztonságos. A Microchip datasheet szerint 4 ms is elég lehet, így 4000 µs fontolható a teljesítmény javítására. Nincs hiba.
// Javaslat: Dokumentáld, hogy ez az írási ciklus várakozási ideje, pl.:
// #define I2C_WRITEDELAY 5000 // Default write cycle time (µs)

#ifndef EN_AUTO_WRITE_PROTECT
#define EN_AUTO_WRITE_PROTECT 0
#endif
// Komment: Az EN_AUTO_WRITE_PROTECT makró valószínűleg az írásvédő pin automatikus kezelését vezérli (0: kikapcsolva). Jelenleg nincs hatása a kódban, mivel a _writeProtectPin-t explicit kezelitek.
// Ellenőrzés: A makró nem használatos, de jövőbeli bővítésre fenntartható. Javaslat: Ha nem tervezel automatikus írásvédelmet, eltávolítható, vagy dokumentáld a célját.
// Javaslat: Dokumentáld, pl.:
// #define EN_AUTO_WRITE_PROTECT 0 // Enable auto write protection (0: disabled)

class I2CEEPROM : public StorageBase {
 public:
  I2CEEPROM(uint8_t i2cAddress, AT24LC ATDEV, TwoWire* wire, int8_t writeProtectPin = -1);
  // Komment: A konstruktor inicializálja az I2C címet, chip típusát (AT24LC), TwoWire objektumot, és opcionális írásvédő pint. Helyes, a paraméterek megfelelő típusúak.
  // Ellenőrzés: A writeProtectPin alapértelmezett értéke -1 jól jelzi, hogy nincs pin megadva. Nincs hiba.
  // Javaslat: Doxygen komment, pl.:
  // /**
  //  * @brief Constructor for I2CEEPROM.
  //  * @param i2cAddress I2C address of the EEPROM (e.g., 0x50).
  //  * @param ATDEV EEPROM chip type (e.g., AT24LC256).
  //  * @param wire TwoWire object for I2C communication.
  //  * @param writeProtectPin Pin for write protection (-1 if not used).
  //  */

  ~I2CEEPROM();
  // Komment: Destruktor, feltételezem üres, mivel nincs dinamikus erőforrás felszabadítás. Helyes.
  // Ellenőrzés: A _wire pointer nem dinamikus, így nincs szükség delete-re. Nincs hiba.

  bool isConnected();
  // Komment: Ellenőrzi, hogy az EEPROM elérhető-e az I2C buszon. Helyes, bool visszatérés megfelelő.
  // Javaslat: Doxygen komment, pl.:
  // /**
  //  * @brief Checks if the EEPROM is connected on the I2C bus.
  //  * @return true if connected, false otherwise.
  //  */

  uint8_t getAddress();
  // Komment: Visszaadja az I2C címet (_i2cAddress). Helyes, uint8_t típus illik az I2C címekhez (0x50-0x57).
  // Javaslat: Doxygen komment.

  virtual uint8_t readu8(uint16_t addr) override;
  virtual bool writeu8(uint16_t addr, uint8_t val) override;
  virtual bool updateu8(uint16_t addr, uint8_t val) override;
  virtual bool format(uint8_t v) override;
  virtual uint32_t getSize() override;
  // Komment: A StorageBase absztrakt metódusainak implementációi, helyesen override-elve. A típusok és visszatérési értékek megfelelőek:
  // - readu8: Egy bájt olvasása, uint8_t visszatérés.
  // - writeu8/updateu8: Egy bájt írása/frissítése, bool sikerjelzés.
  // - format: EEPROM kitöltése egy értékkel, bool siker.
  // - getSize: Eszközméret, uint16_t (max. 65536 bájt).
  // Ellenőrzés: A metódusok szignatúrái illeszkednek a StorageBase-hez. Nincs hiba.
  // Javaslat: Doxygen komment minden metódushoz, pl.:
  // /**
  //  * @brief Reads a single byte from the specified address.
  //  * @param addr Memory address (0 to device size).
  //  * @return The byte read, or 0 if address is invalid.
  //  */

  virtual uint8_t* readArray(uint16_t addr, uint8_t* buffer, size_t length);
  virtual bool writeArray(uint16_t addr, const uint8_t* buffer, size_t length);
  virtual bool updateArray(uint16_t addr, const uint8_t* buffer, size_t length);
  // Komment: Tömbszintű olvasás/írás/frissítés, a StorageBase kiterjesztéseként. Helyes:
  // - readArray: Pointert ad vissza a bufferre, size_t hosszúság kezelésére.
  // - writeArray/updateArray: Bool sikerjelzés, const buffer helyes.
  // Ellenőrzés: A metódusok szignatúrái konzisztensek, és illenek a StorageBase filozófiájához. Nincs hiba.
  // Javaslat: Doxygen komment.

  virtual void flush() override;
  // Komment: Üres flush metódus, helyes, mivel az I2C EEPROM nem igényel külön commit műveletet.
  // Ellenőrzés: Kompatibilis a StorageBase-szel. Nincs hiba.

  bool verifyArray(uint16_t addr, const uint8_t* buffer, uint16_t length);
  // Komment: Ellenőrzi, hogy a tömb helyesen íródott-e, bool visszatéréssel. Hasznos a leállítás előtti írások adatbiztonságához.
  // Javaslat: Doxygen komment.

  uint8_t getPageSize();
  uint8_t getPageSize(AT24LC ATDEV);
  // Komment: Visszaadja az oldal méretet (pl. 64 bájt AT24LC256-hoz). A két overload hasznos: az egyik az aktuális _pageSize-t, a másik az AT24LC alapján számítja.
  // Ellenőrzés: A függvények helyes típusúak (uint8_t). Nincs hiba.

  uint32_t getLastWrite();
  // Komment: Visszaadja az utolsó írás időpontját (mikroszekundumban). Hasznos hibakereséshez vagy időzítés ellenőrzéséhez.
  // Ellenőrzés: A uint32_t típus elég a micros() értékekhez. Nincs hiba.

  void setExtraWriteCycleTime(uint8_t ms);
  uint8_t getExtraWriteCycleTime();
  // Komment: Az extra írási ciklus idő (_extraTWR) beállítása/visszaadása ms-ben. Hasznos a chip-specifikus időzítés finomhangolására.
  // Ellenőrzés: A uint8_t típus elég (255 ms max.), és a _waitATReady használja. Nincs hiba.

 private:
  uint8_t _i2cAddress = 0x50;
  uint32_t _lastWrite = 0;
  uint32_t _deviceSize = 0;
  uint8_t _pageSize = 0;
  uint8_t _extraTWR = 0;
  int8_t _writeProtectPin = -1;
  TwoWire* _wire = nullptr;
  // Komment: A privát tagváltozók helyesen tükrözik az osztály állapotát:
  // - _i2cAddress: I2C cím (pl. 0x50).
  // - _lastWrite: Utolsó írás időpontja (mikroszekundum).
  // - _deviceSize: Eszközméret (bájt).
  // - _pageSize: Oldal méret (pl. 64 bájt).
  // - _extraTWR: Extra várakozási idő (ms).
  // - _writeProtectPin: Írásvédő pin (-1 ha nincs).
  // - _wire: TwoWire pointer az I2C kommunikációhoz.
  // Ellenőrzés: A típusok megfelelőek, és az inicializálások (_lastWrite, _writeProtectPin) helyesek. Nincs hiba.

  void _beginTransmission(const uint16_t addr);
  uint8_t _endTransmission();
  bool _writeArray(uint16_t addr, const uint8_t* buffer, uint16_t length, bool incrBuffer);
  uint8_t _WriteBlock(uint16_t addr, const uint8_t* buffer, uint16_t length);
  uint8_t _ReadBlock(uint16_t addr, uint8_t* buffer, uint16_t length);
  bool _verifyBlock(uint16_t addr, const uint8_t* buffer, uint16_t length);
  void _waitATReady();
  // Komment: Privát segédfüggvények az I2C kommunikációhoz és blokk műveletekhez. Helyes:
  // - _beginTransmission/_endTransmission: I2C tranzakció kezelés.
  // - _writeArray: Tömb írás oldalhatárok figyelembevételével.
  // - _WriteBlock/_ReadBlock: Egy blokk írása/olvasása.
  // - _verifyBlock: Blokk visszaolvasás ellenőrzés.
  // - _waitATReady: Várakozás a chip készenlétére.
  // Ellenőrzés: A függvények szignatúrái megfelelőek, és a visszatérési értékek (bool, uint8_t) konzisztensek. Nincs hiba.
  // Javaslat: Doxygen komment a privát függvényekhez, bár ezek belső használatra vannak.
};
// Komment: Az osztály deklaráció teljes, és illik a StorageBase-hez. A publikus és privát szekciók jól szervezettek.
// Ellenőrzés: Nincs hiányzó metódus vagy tagváltozó. Nincs hiba.