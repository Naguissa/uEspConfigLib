/**
 * \class uEspConfigLibFSLittlefs
 * \brief The deffinitive ESP32 and ESP8266 configuration Arduino library, uEspConfigLib - Littlefs FileSystem interface implementation part
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
 * @file uEspConfigLibFSLittlefs.cpp
 * @copyright Naguissa
 * @author Naguissa
 * @see <a href="https://github.com/Naguissa/uEspConfigLib">https://github.com/Naguissa/uEspConfigLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @version 1.1.0
 */
#include <Arduino.h>
#include "uEspConfigLibFSInterface.h"
#include "uEspConfigLibFSLittlefs.h"


/**
 * \brief Constructor
 *
 * @param init Set to true if you want to initialize and format (if needed) the filesystem
 */
uEspConfigLibFSLittlefs::uEspConfigLibFSLittlefs(const bool init) {
    uEspConfigLibFSLittlefs("/uEspConfig.ini", init);
}
	
/**
 * \brief Constructor
 *
 * @param path File path on filesystem
 * @param init Set to true if you want to initialize and format (if needed) the filesystem
 */
uEspConfigLibFSLittlefs::uEspConfigLibFSLittlefs(const char *path, const bool init) {
    _status = uEspConfigLibFS_STATUS_CLOSED;

    // Copy path
    _path = (char *) malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(_path, path);
    
    // Init FS if requested
	if (init) {
        _status = uEspConfigLibFS_STATUS_NOINIT;
        bool correct = LittleFS.begin(uEspConfigLibFSLittlefs_begin_param);
		yield();
	    if (!correct) {
		    LittleFS.format();
    		yield();
		    correct = LittleFS.begin();
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
bool uEspConfigLibFSLittlefs::openToRead() {
    if (_status == uEspConfigLibFS_STATUS_FATAL) {
        return false;
    }

    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    _dataFile.close();
        _status = uEspConfigLibFS_STATUS_CLOSED;
		yield();
    }
    _dataFile = LittleFS.open(_path, "r");
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
bool uEspConfigLibFSLittlefs::openToWrite() {
    if (_status == uEspConfigLibFS_STATUS_FATAL) {
        return false;
    }

    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    _dataFile.close();
        _status = uEspConfigLibFS_STATUS_CLOSED;
		yield();
    }
    _dataFile = LittleFS.open(_path, "w");
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
bool uEspConfigLibFSLittlefs::readLine(String *line) {
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
bool uEspConfigLibFSLittlefs::write(const char *data) {
    if (_status != uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    return false;
    }
	return (uEspConfigLibFSLittlefs_writeCasted(data) > 0);
}

/**
 * \brief Closes current file
 *
 * @return False on error
 */
bool uEspConfigLibFSLittlefs::closeFile() {
    if (_status == uEspConfigLibFS_STATUS_OPEN_READ || _status == uEspConfigLibFS_STATUS_OPEN_WRITE) {
	    _dataFile.close();
        _status = uEspConfigLibFS_STATUS_CLOSED;
		yield();
        return true;
    }
    return false;
}

