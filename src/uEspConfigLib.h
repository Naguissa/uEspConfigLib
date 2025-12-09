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
 * @file uEspConfigLib.h
 * @copyright Naguissa
 * @author Naguissa
 * @see <a href="https://github.com/Naguissa/uEspConfigLib">https://github.com/Naguissa/uEspConfigLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @version 1.2.0
 */
#pragma once

#include <Arduino.h>
#ifdef ARDUINO_ARCH_ESP32
    #include <WiFi.h>
    #include <WiFiAP.h>
    #include "WebServer.h"
#else
    #include "ESP8266WebServer.h"
#endif
#include "uEspConfigLibFSInterface.h"
#ifdef ARDUINO_ARCH_ESP32
    #include "WebServer.h"
    #define uEspConfigLib_WebServer WebServer
#else
    #include "ESP8266WebServer.h"
    #define uEspConfigLib_WebServer ESP8266WebServer
#endif


/**
 * \brief Regular field
 */
#define uEspConfigLib_OPTION_NONE 0
/**
 * \brief Adds WiFi scanner window to select desired SSID
 */
#define uEspConfigLib_OPTION_SCANNER 1


struct uEspConfigLibList {
    uEspConfigLibList() : next(0), name(0), description(0), defaultValue(0), value(0), option(uEspConfigLib_OPTION_NONE) {};
    uEspConfigLibList *next;
    char * name;
    char * description;
    char * defaultValue;
    char * value;
    uint8_t option;
};

#define uEspConfigLib_free(field) if (field != 0) { free(field); field = 0; }
#define uEspConfigLib_malloc(value) (char *) malloc(sizeof(char) * (strlen(value) + 1))
#define uEspConfigLib_WebServer_sendContent(data) if (*data != 0) { server->sendContent(data); }

class uEspConfigLib {
    public:
		/**
		 * \brief Constructor
		 *
		 * @param fs Selected uEspConfigLibFSInterface to be used
		 */
        uEspConfigLib(uEspConfigLibFSInterface *);
        
        // Regular usage functions
		/**
		 * \brief Defines a configuration option
		 *
		 * @param name Name of configuration option
		 * @param description Description of the configuration option
		 * @param defaultValue Default value of the configuration option
		 * @param option Optional. Special option, uEspConfigLib_OPTION_NONE by default
		 */
        void addOption(const char *, const char *, const char *, const uint8_t = uEspConfigLib_OPTION_NONE);

        /**
         * \brief Changes a configuration option current value
         *
         * @param name Name of configuration option
         * @param value Default value of the configuration option
         * @return False on error (name not defined previously)
         */
        bool set(const char *, const char *);

        /**
         * \brief Gets a char pointer to the current value of a configuration option
         *
         * @param name Name of configuration option
         * @return Char pointer. Null if not found.
         */
        char * getPointer(const char *);

        /**
         * \brief Changes a configuration option to its default value
         *
         * @param name Name of configuration option
         * @return False on error (name not defined previously)
         */
        bool clear(const char *);

        // Web server functions
        /**
         * \brief Handles a configuration HTML form request
         *
         * This will handle the request and serve an HTML page with a form with all configurations.
         *
         * @param server WebServer or ESP8266WebServer variable, depending on architecture
         * @param path Path where the form will be sent
         */
        void handleConfigRequestHtml(uEspConfigLib_WebServer *, const char * = "/uConfigLib/saveConfig");

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
        void handleConfigRequestJson(uEspConfigLib_WebServer *);

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
        void handleSaveConfig(uEspConfigLib_WebServer *);
        
        
        // FS functions

        /**
         * \brief Loads config file from FileSystem and processes it
         *
         * @return False on error
         */
        bool loadConfigFile();

        /**
         * \brief Recreates and overwrites config file to FileSystem with current configurations
         *
         * @return False on error
         */
        bool saveConfigFile();

    private:
        uEspConfigLib(); // 1 arg needed
        void _copyName(uEspConfigLibList *, const char *);
        void _copyDefault(uEspConfigLibList *, const char *);
        void _copyValue(uEspConfigLibList *, const char *);
        void _copyValue(uEspConfigLibList *, const String);
        void _copyDescription(uEspConfigLibList *, const char *);
        void _parseConfigLine(String);
        void _handleWifiScan(uEspConfigLib_WebServer *, const String);
        void handleWifiScanResult();
        uEspConfigLibList *list;
        uEspConfigLibFSInterface * _fs;
};

