/**
 * \class uEspConfigLibFSInterface
 * \brief The deffinitive ESP32 and ESP8266 configuration Arduino library, uEspConfigLib - FileSystem interface part
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
 * @file uEspConfigLibFSInterface.h
 * @copyright Naguissa
 * @author Naguissa
 * @see <a href="https://github.com/Naguissa/uEspConfigLib">https://github.com/Naguissa/uEspConfigLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @version 1.0.0
 */
#pragma once

#include <Arduino.h>

/**
 * \brief FileSystem class status: UNITIALIZED
 */
#define uEspConfigLibFS_STATUS_NOINIT 0
/**
 * \brief FileSystem class status: FILE CLOSED
 */
#define uEspConfigLibFS_STATUS_CLOSED 1
/**
 * \brief FileSystem class status: FILE OPEN TO READ
 */
#define uEspConfigLibFS_STATUS_OPEN_READ 2
/**
 * \brief FileSystem class status: FILE OPEN TO WRITE
 */
#define uEspConfigLibFS_STATUS_OPEN_WRITE 3
/**
 * \brief FileSystem class status: TEMPORARY ERROR
 */
#define uEspConfigLibFS_STATUS_ERROR 9
/**
 * \brief FileSystem class status: FATAL ERROR
 */
#define uEspConfigLibFS_STATUS_FATAL 10

class uEspConfigLibFSInterface {
    public:
        // Regular usage functions
        /**
         * \brief Opens the file for read
         *
         * @return False on error
         */
        virtual bool openToRead() { return false; };

        /**
         * \brief Opens the file for write
         *
         * @return False on error
         */
        virtual bool openToWrite() { return false; };

        /**
         * \brief Reads a whole line from current file
         *
         * @param line String pointer where line will be read
         * @return False on error
         */
        virtual bool readLine(String *) { return false; };

        /**
         * \brief Writes content to current file
         *
         * @param data char array pointer to be written
         * @return False on error
         */
        virtual bool write(const char *) { return false; };

        /**
         * \brief Closes current file
         *
         * @return False on error
         */
        virtual bool closeFile() { return false; };

        /**
         * \brief Returns current status
         *
         * @return Current status
         */
        uint8_t status() { return _status; }

    private:
        uint8_t _status;
        char * _path = 0;
};

