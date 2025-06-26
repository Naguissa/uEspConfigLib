/**
 * \class uEspConfigLibFSEEPROM
 * \brief The deffinitive ESP32 and ESP8266 configuration Arduino library, uEspConfigLib - EEPROM FileSystem interface implementation part
 *
 * This library consist in 2 parts:
 *
 * One main library, uEspConfigLib, that manages configuration options, defaults, configuration site (HTML and JSON), configuration processing (from a POST of previous site) and configuration storage.
 *
 * One interface to manage different configuration storages.
 * 
 * Currently storage classes are: SD-card, LittleFS, SPIFFS and none (values are lost after restart).
 *
 *
 * @file uEspConfigLibFSEEPROM.h
 * @copyright Naguissa
 * @author Naguissa
 * @see <a href="https://github.com/Naguissa/uEspConfigLib">https://github.com/Naguissa/uEspConfigLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @version 1.1.0
 */
#pragma once

#include <Arduino.h>
#include "uEspConfigLibFSInterface.h"
#include "uEEPROMLib.h"


class uEspConfigLibFSEEPROM : public uEspConfigLibFSInterface {

    #define uEspConfigLibFSEEPROM_BUFFER_SIZE 128

    public:
        /**
         * \brief Constructor
         *
         * @param init Set to true if you want to initialize and format (if needed) the filesystem
         * @param address EEPROM I2C address
         */
        uEspConfigLibFSEEPROM(const bool=false, const uint8_t = UEEPROMLIB_ADDRESS);

        /**
         * \brief Constructor
         *
         * @param path File path on filesystem
         * @param init Set to true if you want to initialize and format (if needed) the filesystem
         * @param address EEPROM I2C address
         */
        uEspConfigLibFSEEPROM(const char*, const bool=false, const uint8_t = UEEPROMLIB_ADDRESS);

        /**
         * \brief Opens the file for read
         *
         * @return False on error
         */
        bool openToRead();

        /**
         * \brief Opens the file for write
         *
         * @return False on error
         */
        bool openToWrite();

        /**
         * \brief Reads a whole line from current file
         *
         * @param line String pointer where line will be read
         * @return False on error
         */
        bool readLine(String *);

        /**
         * \brief Writes content to current file
         *
         * @param data char array pointer to be written
         * @return False on error
         */
        bool write(const char *);

        /**
         * \brief Closes current file
         *
         * @return False on error
         */
        bool closeFile();

    private:
        bool _writeFlush();
        byte _buffer[uEspConfigLibFSEEPROM_BUFFER_SIZE];
        unsigned int _position = 0;
        unsigned int _bufferPosition = 0;
        uint8_t _status;
        bool _saveInComment = false;
        bool _saveLineStart = true;
        uEEPROMLib * _eeprom;
    };

