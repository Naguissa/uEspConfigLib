/**
 * \class uEspConfigLibFSEEPROM
 * \brief The deffinitive ESP32 and ESP8266 configuration Arduino library, uEspConfigLib - EEPROM writing implementation part
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
 * @file uEspConfigLibFSEEPROM.cpp
 * @copyright Naguissa
 * @author Naguissa
 * @see <a href="https://github.com/Naguissa/uEspConfigLib">https://github.com/Naguissa/uEspConfigLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @version 1.1.0
 */
#include <Arduino.h>
#include "uEspConfigLibFSInterface.h"
#include "uEspConfigLibFSEEPROM.h"


/**
 * \brief Constructor
 *
 * @param init Set to true if you want to initialize and format (if needed) the filesystem
 * @param address EEPROM I2C address
 */
uEspConfigLibFSEEPROM::uEspConfigLibFSEEPROM(const bool init, const uint8_t address) {
    uEspConfigLibFSEEPROM("", init, address);
}
	
/**
 * \brief Constructor
 *
 * @param path File path on filesystem
 * @param init Set to true if you want to initialize and format (if needed) the filesystem
 * @param address EEPROM I2C address
 */
uEspConfigLibFSEEPROM::uEspConfigLibFSEEPROM(const char *path, const bool init, const uint8_t address) {
    _eeprom = new uEEPROMLib(address);
    _status = uEspConfigLibFS_STATUS_CLOSED;
}

/**
 * \brief Opens the file for read
 *
 * @return False on error
 */
bool uEspConfigLibFSEEPROM::openToRead() {
    if (_status == uEspConfigLibFS_STATUS_FATAL) {
        return false;
    }
    _position = 0;
    _bufferPosition = 0;
    _status = uEspConfigLibFS_STATUS_OPEN_READ;
    
	return true;
}


/**
 * \brief Opens the file for write
 *
 * @return False on error
 */
bool uEspConfigLibFSEEPROM::openToWrite() {
    if (_status == uEspConfigLibFS_STATUS_FATAL) {
        return false;
    }
    _position = 0;
    _bufferPosition = 0;
    _saveInComment = false;
    _saveLineStart = true;
    _status = uEspConfigLibFS_STATUS_OPEN_WRITE;

	return true;
}


/**
 * \brief Reads a whole line from current file
 *
 * @param line String pointer where line will be read
 * @return False on error
 */
bool uEspConfigLibFSEEPROM::readLine(String *line) {
    if (_status != uEspConfigLibFS_STATUS_OPEN_READ) {
	    return false;
    }
    char actualChar;

    line->remove(0);
    // Limit to 50Kb lines, more than expected but at least we add one exit path
    for (int loop = 0; loop < 400; loop++) {
        if (!_eeprom->eeprom_read(_position, _buffer, uEspConfigLibFSEEPROM_BUFFER_SIZE)) {
            return false;
        }
        for (unsigned int i = 0; i < uEspConfigLibFSEEPROM_BUFFER_SIZE; i++) {
        actualChar = (char) _buffer[i];
            line->concat(actualChar);
            _position++;
    		yield();
            if (actualChar == '\0') { // EOF
                return false;
            }
            if (actualChar == '\n' || actualChar == '\r') { // EOL
                return (line->length() > 0);
            }
        }
    }
    return false;
}

/**
 * \brief Writes content to current file
 *
 * @param data char array pointer to be written
 * @return False on error
 */
bool uEspConfigLibFSEEPROM::write(const char *data) {
    if (_status != uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    return false;
    }
    int len = strlen(data);
    for (int i = 0; i < len; i++) {
        if (_bufferPosition >= uEspConfigLibFSEEPROM_BUFFER_SIZE) {
            if (!_writeFlush()) {
                return false;
            }
        }
        if(data[i] == '\0') { // Should not happen but skip it
            continue;
        }
        if(_saveLineStart && (data[i] == '#' || data[i] == ';' || (data[i] == '/' && i < len && data[i+1] == '/'))) {
            _saveInComment = true;
            continue;
        }
        if (!_saveInComment) {
            _buffer[_bufferPosition] = data[i];
            _bufferPosition++;
            _saveLineStart = false;
        }

        if (data[i] == '\n' || data[i] == '\r') {
            _saveInComment = false;
            _saveLineStart = true;
            continue;
        }
        
        
    }
	return true;
}


/**
 * \brief Writes buffer content to EEPROM
 *
 * @return False on error
 */
bool uEspConfigLibFSEEPROM::_writeFlush() {
    unsigned int size = _bufferPosition;
    
    if (_status != uEspConfigLibFS_STATUS_OPEN_WRITE || _bufferPosition == 0) {
        return false;
    }
    if(_bufferPosition < uEspConfigLibFSEEPROM_BUFFER_SIZE) {
        _buffer[_bufferPosition] = 0; // Add EOF
        size++;
    }
    bool ret = _eeprom->eeprom_write(_position, (void *) _buffer, size);
    _position += _bufferPosition;
    _bufferPosition = 0;
    return ret;
}



/**
 * \brief Closes current file
 *
 * @return False on error
 */
bool uEspConfigLibFSEEPROM::closeFile() {
    if (_status == uEspConfigLibFS_STATUS_OPEN_WRITE && _bufferPosition > 0) {
        _status = uEspConfigLibFS_STATUS_CLOSED;
        return _writeFlush();
    }
    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {    
        _status = uEspConfigLibFS_STATUS_CLOSED;
        return true;
    }
    return false;
}

