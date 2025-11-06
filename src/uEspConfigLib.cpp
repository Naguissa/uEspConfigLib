/**
 * \class uEspConfigLib
 * \brief The deffinitive ESP32 and ESP8266 configuration Arduino library, uEspConfigLib
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
 * @file uEspConfigLib.cpp
 * @copyright Naguissa
 * @author Naguissa
 * @see <a href="https://github.com/Naguissa/uEspConfigLib">https://github.com/Naguissa/uEspConfigLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @version 1.2.0
 */
#include <Arduino.h>
#include "uEspConfigLib.h"
#include "uEspConfigLibFSInterface.h"
#ifdef ARDUINO_ARCH_ESP32
    #include "WebServer.h"
#else
    #include "ESP8266WebServer.h"
#endif


/**
 * \brief Constructor
 *
 * @param fs Selected uEspConfigLibFSInterface to be used
 */
uEspConfigLib::uEspConfigLib(uEspConfigLibFSInterface * fs) {
    _fs = fs;
    list = 0;
}

void uEspConfigLib::_copyName(uEspConfigLibList * slot, const char * value) {
    uEspConfigLib_free(slot->name);
    slot->name = (char *) malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(slot->name, value);
}

void uEspConfigLib::_copyDefault(uEspConfigLibList * slot, const char * value) {
    uEspConfigLib_free(slot->defaultValue);
    slot->defaultValue = (char *) malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(slot->defaultValue, value);
}

void uEspConfigLib::_copyValue(uEspConfigLibList * slot, const char * value) {
    uEspConfigLib_free(slot->value);
    slot->value = (char *) malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(slot->value, value);
}

void uEspConfigLib::_copyValue(uEspConfigLibList * slot, const String value) {
    uEspConfigLib_free(slot->value);
    slot->value = (char *) malloc(sizeof(char) * (value.length() + 1));
    value.toCharArray(slot->value, value.length() + 1);
}

void uEspConfigLib::_copyDescription(uEspConfigLibList * slot, const char * value) {
    uEspConfigLib_free(slot->description);
    slot->description = (char *) malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(slot->description, value);
}



/**
 * \brief Defines a configuration option
 *
 * @param name Name of configuration option
 * @param description Description of the configuration option
 * @param defaultValue Default value of the configuration option
 */
void uEspConfigLib::addOption(const char * name, const char * description, const char * defaultValue, const uint8_t option) {
    if (list == 0) {
        list = new uEspConfigLibList;
        _copyName(list, name);
        _copyDescription(list, description);
        _copyDefault(list, defaultValue);
        _copyValue(list, defaultValue);        
        list->option = option;
        list->next = 0;
        return;
    }
    
    uEspConfigLibList * prev = list;
    for (uEspConfigLibList * slot = list; slot != 0; slot = slot->next) {
        if (strcmp(slot->name, name) == 0) {
            _copyDescription(slot, description);
            _copyDefault(slot, defaultValue);
            _copyValue(slot, defaultValue);        
            list->option = option;
            return;
        }
        prev = slot;
    }
    
    prev->next = new uEspConfigLibList;
    _copyName(prev->next, name);
    _copyDescription(prev->next, description);
    _copyDefault(prev->next, defaultValue);
    _copyValue(prev->next, defaultValue);        
    prev->next->option = option;
    prev->next->next = 0;
}


/**
 * \brief Changes a configuration option current value
 *
 * @param name Name of configuration option
 * @param value Default value of the configuration option
 * @return False on error (name not defined previously)
 */
bool uEspConfigLib::set(const char * name, const char *value) {
    if (list == 0) {
        return false;
    }
    
    for (uEspConfigLibList * slot = list; slot != 0; slot = slot->next) {
        if (strcmp(slot->name, name) == 0) {
            _copyValue(slot, value);
            return true;
        }
    }
    return false;
}

/**
 * \brief Gets a char pointer to the current value of a configuration option
 *
 * @param name Name of configuration option
 * @return Char pointer. Null if not found.
 */
char * uEspConfigLib::getPointer(const char * name) {
    for (uEspConfigLibList * slot = list; slot != 0; slot = slot->next) {
        if (strcmp(slot->name, name) == 0) {
            return slot->value;
        }
    }
    return 0;
}

/**
 * \brief Changes a configuration option to its default value
 *
 * @param name Name of configuration option
 * @return False on error (name not defined previously)
 */
bool uEspConfigLib::clear(const char *name) {
    uEspConfigLibList *slot;
    for (slot = list; slot != 0; slot = slot->next) {
        if (strcmp(slot->name, name) == 0) {
            _copyValue(slot, slot->defaultValue);
            return true;
        }
    }
    return false;
}

/**
 * \brief Handles a configuration HTML form request
 *
 * This will handle the request and serve an HTML page with a form with all configurations.
 *
 * @param server WebServer or ESP8266WebServer variable, depending on architecture
 * @param path Path where the form will be sent
 */
void uEspConfigLib::handleConfigRequestHtml(uEspConfigLib_WebServer * server, const char *path) {
    if(server->arg("option") == "scan" && server->arg("field").length() > 0) {
        _handleWifiScan(server, server->arg("field"));
        return;
    }

    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    yield();
    server->send(200, "text/html", "");
    yield();
    server->sendContent("<html><head><title>IoT device config - uConfigLib</title></head><body><form method=\"POST\" action=\"");
    yield();
    server->sendContent(path);
    yield();
    server->sendContent("\"><table border=\"0\">");
    yield();
    uEspConfigLibList *slot;
    for (slot = list; slot != 0; slot = slot->next) {
        server->sendContent("<tr><td colspan=\"2\"><br>");
        yield();
        uEspConfigLib_WebServer_sendContent(slot->description);
        yield();
        server->sendContent(":</td></tr><tr><td>");
        yield();
        uEspConfigLib_WebServer_sendContent(slot->name);
        yield();
        server->sendContent(" (default: ");
        yield();
        uEspConfigLib_WebServer_sendContent(slot->defaultValue);
        yield();
        server->sendContent("):</td><td><input type=\"text\" name=\"");
        yield();
        uEspConfigLib_WebServer_sendContent(slot->name);
        yield();
        if (slot->option == uEspConfigLib_OPTION_SCANNER) {
            server->sendContent("\" id=\"");
            yield();
            uEspConfigLib_WebServer_sendContent(slot->name);
            yield();
        }
        server->sendContent("\" value=\"");
        yield();
        uEspConfigLib_WebServer_sendContent(slot->value);
        yield();
        server->sendContent("\">");
        yield();
        if (slot->option == uEspConfigLib_OPTION_SCANNER) {
            server->sendContent(" <a href=\"javascript:window.open('?option=scan&field=");
            uEspConfigLib_WebServer_sendContent(slot->name);
            server->sendContent("')\">Scan</a>");
            yield();
        }        
        server->sendContent("</td></tr>");
        yield();
    }
    server->sendContent("<tr><td colspan=\"2\"><br><center><button type=\"submit\">Send</button></center></td></tr></table></body>");
    yield();
}

/**
 * \brief Handles a configuration JSON request
 *
 * This will handle the request and serve a JSON document with all configurations:
 *
 * {
 *     "data": [
 *         {
 *             "name": "variable_name",
 *             "description": "variable_description",
 *             "defaultValue": "variable_default_value",
 *             "value": "variable_current_value"
 *         },
 *         {
 *             "name": "variable_name",
 *             "description": "variable_description",
 *             "defaultValue": "variable_default_value",
 *             "value": "variable_current_value"
 *         },
 *         {
 *             "name": "variable_name",
 *             "description": "variable_description",
 *             "defaultValue": "variable_default_value",
 *             "value": "variable_current_value"
 *         }
 *     ]
 * }
 *
 * @param server WebServer or ESP8266WebServer variable, depending on architecture
 */
void uEspConfigLib::handleConfigRequestJson(uEspConfigLib_WebServer * server) {
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    yield();
	server->send(200, "application/vnd.api+json", "");
    yield();
    server->sendContent("{\"data\":[");
    yield();
    bool first = true;
    for (uEspConfigLibList *slot = list; slot != 0; slot = slot->next) {
        if (first) {
            first = false;
        } else {
            server->sendContent(",");
            yield();
        }
        server->sendContent("{\"name\":\"");
        yield();
        uEspConfigLib_WebServer_sendContent(slot->name);
        yield();
        server->sendContent("\", \"description\":\"");
        yield();
        uEspConfigLib_WebServer_sendContent(slot->description);
        yield();
        server->sendContent("\", \"defaultValue\":\"");
        yield();
        uEspConfigLib_WebServer_sendContent(slot->defaultValue);
        yield();
        server->sendContent("\", \"value\":\"");
        yield();
        uEspConfigLib_WebServer_sendContent(slot->value);
        yield();
        server->sendContent("\"}");
        yield();
    }
    server->sendContent("]}");
}


/**
 * \brief Handles a save configuration request
 *
 * This will handle the request and parameters, change any existing one and save the config file.
 * If a config option is not set as parameter will be ignored. If a config option is empty it will be stored as empty.
 * It usually will respond with a 302 redirect to /?saved=1, but if "format" paramer is passed with "json" value it will respond as JSON:
 *  {
 *     "data": {
 *         "result": 1
 *     }
 * }
 *
 * @param server WebServer or ESP8266WebServer variable, depending on architecture
 */
void uEspConfigLib::handleSaveConfig(uEspConfigLib_WebServer * server) {
    String value;
    bool isJson;

    value = server->arg("format");
    isJson = (value == "json");
    
    for (uEspConfigLibList *slot = list; slot != 0; slot = slot->next) {
        if (server->hasArg(slot->name)) {
            value = server->arg(slot->name);
            value.trim();
            _copyValue(slot, value.c_str());
        }
    }
    saveConfigFile();
    
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    if (isJson) {
        server->send(200, "application/vnd.api+json", "");
	    server->sendContent("{\"data\":{\"result\": 1}}");
    } else {
      server->sendHeader("Location", "/?saved=1", true);
      server->send(302, "text/html","");
    }
}


void uEspConfigLib::_parseConfigLine(String line) {
    int pos;
    String variable, value;
    line.trim();
    if (line.startsWith(";") || line.startsWith("#") || line.startsWith("//")) { // comment line
	    return;
    }
    pos = line.indexOf('=');
    if (pos < 2) { // Not found or too short for sure, skip
	    return;
    }
    variable = line.substring(0, pos - 1);
    value = line.substring(pos + 1);
    variable.trim();
    value.trim();

    set(variable.c_str(), value.c_str());
}


/**
 * \brief Loads config file from FileSystem and processes it
 *
 * @return False on error
 */
bool uEspConfigLib::loadConfigFile() {
    bool result;
	String line;
    result = _fs->openToRead();
    if (!result) {
        return false;
    }
    while (_fs->readLine(&line)) {
	    _parseConfigLine(line);
    }
    _fs->closeFile();
    return true;
}

/**
 * \brief Recreates and overwrites config file to FileSystem with current configurations
 *
 * @return False on error
 */
bool uEspConfigLib::saveConfigFile() {
    bool result;
    
    result = _fs->openToWrite();
    if (!result) {
        return false;
    }
    _fs->write("# uConfigLib Configuration file\n");
    yield();
    _fs->write("# Visit https://github.com/Naguissa/uEspConfigLib for details\n");
    yield();
    _fs->write("# Syntax:\n");
    yield();
	_fs->write("# variable = value\n");
    yield();
	_fs->write("# (spaces and empty lines doesn't care)\n");
    yield();
	_fs->write("# This file comments: lines starting with #, ; or //\n");
    yield();
	
    for (uEspConfigLibList * slot = list; slot != 0; slot = slot->next) {
        _fs->write("\n# ");
        yield();
        _fs->write(slot->name);
        yield();
        _fs->write(" -- ");
        yield();
        _fs->write(slot->description);
        yield();
        _fs->write("\n# Default value: ");
        yield();
        _fs->write(slot->defaultValue);
        yield();
        _fs->write("\n");
        yield();
        _fs->write(slot->name);
        yield();
        _fs->write(" = ");
        yield();
        _fs->write(slot->value);
        yield();
        _fs->write("\n");
        yield();
    }
    _fs->closeFile();
	yield();
	return true;
}

void uEspConfigLib::_handleWifiScan(uEspConfigLib_WebServer * server, const String field) {
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    yield();
    server->send(200, "text/html", "");
    yield();
    server->sendContent("<html><head><title>IoT device config - uConfigLib</title></head><body><p><b>WiFi networks:</b></p>");
    yield();

    int n = WiFi.scanNetworks();
    if (n == 0) {
        server->sendContent("<p><i>No networks found</i></p>");
        yield();
    } else {
        server->sendContent("<ul>");
        for (int i = 0; i < n; i++) {
            server->sendContent("<li><a href=\"javascript:window.opener.document.getElementById('" + field + "').value='" + WiFi.SSID(i) + "';window.close();\">" + WiFi.SSID(i) + " - Channel: " + WiFi.channel(i) + " - RSSI: " + WiFi.RSSI(i) + " - Encription: ");
            yield();
            switch (WiFi.encryptionType(i)) {
                case ENC_TYPE_WEP: server->sendContent("WEP"); break;
                case ENC_TYPE_TKIP: server->sendContent("WPA/PSK"); break;
                case ENC_TYPE_CCMP: server->sendContent("WPA2/PSK"); break;
                case ENC_TYPE_NONE: server->sendContent("NONE"); break;
                case ENC_TYPE_AUTO: server->sendContent("AUTO (WPA/WPA2/PSK)"); break;
                default: server->sendContent("Unknown"); break;
            }
            server->sendContent("</a></li>");
            yield();
        }
        server->sendContent("</ul>");
    }
}


