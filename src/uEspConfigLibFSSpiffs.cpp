/**
 * \class uEspConfigLibFSSpiffs
 * \brief The deffinitive ESP32 and ESP8266 configuration Arduino library, uEspConfigLib - SPIFFS FileSystem interface implementation part
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
 * @file uEspConfigLibFSSpiffs.cpp
 * @copyright Naguissa
 * @author Naguissa
 * @see <a href="https://github.com/Naguissa/uEspConfigLib">https://github.com/Naguissa/uEspConfigLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @version 1.1.0
 */#include <Arduino.h>
#include "uEspConfigLibFSInterface.h"
#include "uEspConfigLibFSSpiffs.h"


/**
 * \brief Constructor
 *
 * @param init Set to true if you want to initialize and format (if needed) the filesystem
 */
uEspConfigLibFSSpiffs::uEspConfigLibFSSpiffs(const bool init) {
    uEspConfigLibFSSpiffs("/uEspConfig.ini", init);
}
	
/**
 * \brief Constructor
 *
 * @param path File path on filesystem
 * @param init Set to true if you want to initialize and format (if needed) the filesystem
 */
uEspConfigLibFSSpiffs::uEspConfigLibFSSpiffs(const char *path, const bool init) {
    _status = uEspConfigLibFS_STATUS_CLOSED;

    // Copy path
    _path = (char *) malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(_path, path);
    
    // Init FS if requested
	if (init) {
        _status = uEspConfigLibFS_STATUS_NOINIT;
	    bool correct = SPIFFS.begin();
		yield();
	    if (!correct) {
		    SPIFFS.format();
    		yield();
		    correct = SPIFFS.begin();
    		yield();
	    }
	    if (correct) {
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
bool uEspConfigLibFSSpiffs::openToRead() {
    if (_status == uEspConfigLibFS_STATUS_FATAL) {
        return false;
    }

    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    _dataFile.close();
        _status = uEspConfigLibFS_STATUS_CLOSED;
		yield();
    }
    _dataFile = SPIFFS.open(_path, "r");
	yield();
    if (!_dataFile) {
        _status = uEspConfigLibFS_STATUS_ERROR;
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
bool uEspConfigLibFSSpiffs::openToWrite() {
    if (_status == uEspConfigLibFS_STATUS_FATAL) {
        return false;
    }

    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    _dataFile.close();
        _status = uEspConfigLibFS_STATUS_CLOSED;
		yield();
    }
    _dataFile = SPIFFS.open(_path, "w");
		yield();
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
bool uEspConfigLibFSSpiffs::readLine(String *line) {
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
bool uEspConfigLibFSSpiffs::write(const char *data) {
    if (_status != uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    return false;
    }
	return (uEspConfigLibFSSpiffs_writeCasted(data) > 0);
}

/**
 * \brief Closes current file
 *
 * @return False on error
 */
bool uEspConfigLibFSSpiffs::closeFile() {
    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    _dataFile.close();
        _status = uEspConfigLibFS_STATUS_CLOSED;
		yield();
        return true;
    }
    return false;
}

