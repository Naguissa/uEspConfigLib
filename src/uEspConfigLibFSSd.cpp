/**
 * \class uEspConfigLibFSSd
 * \brief The deffinitive ESP32 and ESP8266 configuration Arduino library, uEspConfigLib - SD-card FileSystem interface implementation part
 *
 * This library consist in 2 parts:
 *
 * One main library, uEspConfigLib, that manages configuration options, defaults, configuration site (HTML and JSON), configuration processing (from a POST of previous site) and configuration storage.
 *
 * One interface to manage different configuration storages.
 * 
 * Currently storage classes are: SD-card, LittleFS, SPIFFS, EEPROM and none (values are lost after restart).
 *
 *
 * @file uEspConfigLibFSSd.cpp
 * @copyright Naguissa
 * @author Naguissa
 * @see <a href="https://github.com/Naguissa/uEspConfigLib">https://github.com/Naguissa/uEspConfigLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @version 1.1.0
 */
#include <Arduino.h>
#include "uEspConfigLibFSInterface.h"
#include "uEspConfigLibFSSd.h"


/**
 * \brief Constructor
 *
 * @param init Set to true if you want to initialize and format (if needed) the filesystem
 */
uEspConfigLibFSSd::uEspConfigLibFSSd(const bool init) {
    uEspConfigLibFSSd("/uEspConfig.ini", init, SS);
}
	
/**
 * \brief Constructor
 *
 * @param path File path on filesystem
 * @param init Set to true if you want to initialize and format (if needed) the filesystem
 * @param pin CS pin, if not the standard one
 */
uEspConfigLibFSSd::uEspConfigLibFSSd(const char * path, const bool init, const uint8_t pin) {
    _status = uEspConfigLibFS_STATUS_CLOSED;

    // Copy path
    _path = (char *) malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(_path, path);
    
    // Init FS if requested
	if (init) {
	    if (SD.begin(pin)) {
            _status = uEspConfigLibFS_STATUS_CLOSED;
	    } else {
            _status = uEspConfigLibFS_STATUS_FATAL;
	    }
	}
}

/**
 * \brief Opens the file for read
 *
 * @return False on error
 */
bool uEspConfigLibFSSd::openToRead() {
    if (_status == uEspConfigLibFS_STATUS_FATAL) {
        return false;
    }

    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    _dataFile.close();
        _status = uEspConfigLibFS_STATUS_CLOSED;
		yield();
    }
    
	_dataFile = SD.open(_path, FILE_READ);
	if (_dataFile.isDirectory()) {
		_dataFile.close();
        _status = uEspConfigLibFS_STATUS_FATAL;
		return false;
	}
    _status = uEspConfigLibFS_STATUS_OPEN_READ;
	return true;
}


/**
 * \brief Opens the file for write
 *
 * @return False on error
 */
bool uEspConfigLibFSSd::openToWrite() {
    if (_status == uEspConfigLibFS_STATUS_FATAL) {
        return false;
    }

    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    _dataFile.close();
        _status = uEspConfigLibFS_STATUS_CLOSED;
		yield();
    }
    
	_dataFile = SD.open(_path, FILE_WRITE);
    if (!_dataFile) {
        _status = uEspConfigLibFS_STATUS_ERROR;
        return false;
    }
    _status = uEspConfigLibFS_STATUS_OPEN_WRITE;
	return true;
}


/**
 * \brief Reads a whole line from current file
 *
 * @param line String pointer where line will be read
 * @return False on error
 */
bool uEspConfigLibFSSd::readLine(String *line) {
    if (_status != uEspConfigLibFS_STATUS_OPEN_READ) {
	    return false;
    }
	if (_dataFile.available()) {    
		yield();
	    *line = _dataFile.readStringUntil('\n');
		yield();
        return true;
    }
    return false;
}

/**
 * \brief Writes content to current file
 *
 * @param data char array pointer to be written
 * @return False on error
 */
bool uEspConfigLibFSSd::write(const char *data) {
    if (_status != uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    return false;
    }
	return (uEspConfigLibFSSd_writeCasted(data) > 0);
}

/**
 * \brief Closes current file
 *
 * @return False on error
 */
bool uEspConfigLibFSSd::closeFile() {
    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    _dataFile.close();
        _status = uEspConfigLibFS_STATUS_CLOSED;
		yield();
        return true;
    }
    return false;
}

