/**
 * \brief The deffinitive ESP32 and ESP8266 configuration Arduino library, uEspConfigLib - Example sketch
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
 * @file uEspConfigLib_example.ino
 * @copyright Naguissa
 * @author Naguissa
 * @see <a href="https://github.com/Naguissa/uEspConfigLib">https://github.com/Naguissa/uEspConfigLib</a>
 * @see <a href="mailto:naguissa@foroelectro.net">naguissa@foroelectro.net</a>
 * @version 1.2.0
 */

#include "Arduino.h"

#include "uEspConfigLibFSNone.h"
#include "uEspConfigLibFSSpiffs.h"
#include "uEspConfigLibFSLittlefs.h"
#include "uEspConfigLibFSSd.h"
#include "uEspConfigLibFSEEPROM.h"
#include "uEspConfigLib.h"



// Caution: It need to be a global variable or a global pointer.
// if FS is a 'setup' variable it will lead to crashes
uEspConfigLibFSInterface * configFsA;
uEspConfigLibFSInterface * configFsB;
uEspConfigLibFSInterface * configFsC;
uEspConfigLibFSInterface * configFsD;
uEspConfigLib * config;


#ifdef ARDUINO_ARCH_ESP32
    WebServer server(80);
#else
    ESP8266WebServer server(80);
#endif


/// Available functions:
void setupWiFi();
void setup();
void loop();
void handleGetConfigHtml();
void handleGetConfigJson();
void handleSaveConfig();
void handleDefault();



void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("init");
    delay(1500);

    Serial.println(" - SETUP -");
   

    configFsA = new uEspConfigLibFSNone("/config.ini", true);
    if (configFsA->status() == uEspConfigLibFS_STATUS_FATAL) {
        Serial.println("  * Error initializing FS none");
    }
    // Cannot use SPIFFS and LittleFS at same time
    //configFsB = new uEspConfigLibFSSpiffs("/config.ini", true);
    //if (configFsB->status() == uEspConfigLibFS_STATUS_FATAL) {
    //    Serial.println("  * Error initializing FS SPIFFS");
    //}
    configFsC = new uEspConfigLibFSLittlefs("/config.ini", true);
    if (configFsC->status() == uEspConfigLibFS_STATUS_FATAL) {
        Serial.println("  * Error initializing FS LittleFS");
    }
    configFsD = new uEspConfigLibFSSd("/config.ini", true);
    if (configFsD->status() == uEspConfigLibFS_STATUS_FATAL) {
        Serial.println("  * Error initializing FS SD");
    }
    
    config = new uEspConfigLib(configFsC);

    char * readResult;

    Serial.println("\n - configure -");    
    // 1st step is to define your variables: Name, Description, Default value
    config->addOption("wifi_mode", "WiFi mode (C=Client, other=Access Point)", "");
    config->addOption("wifi_ssid", "SSID of your WiFi", "Unconfigured_device", uEspConfigLib_OPTION_SCANNER);
    config->addOption("wifi_password", "Password of your WiFi", "wifi_password");
    

    Serial.println("\n - default values -");    

    readResult = config->getPointer("wifi_mode");
    Serial.print("wifi_mode: ");
    Serial.println(readResult);
    readResult = config->getPointer("wifi_ssid");
    Serial.print("wifi_ssid: ");
    Serial.println(readResult);
    readResult = config->getPointer("wifi_password");
    Serial.print("wifi_password: ");
    Serial.println(readResult);


    config->loadConfigFile();
    Serial.println("\n - loaded values -");    

    readResult = config->getPointer("wifi_mode");
    Serial.print("wifi_mode: ");
    Serial.println(readResult);
    readResult = config->getPointer("wifi_ssid");
    Serial.print("wifi_ssid: ");
    Serial.println(readResult);
    readResult = config->getPointer("wifi_password");
    Serial.print("wifi_password: ");
    Serial.println(readResult);


    Serial.println("\n - manually set values -");    

    config->set("wifi_mode", "A");
    config->set("wifi_ssid", "uEspConfigLib_EXAMPLE");
    config->set("wifi_password", "");

    readResult = config->getPointer("wifi_mode");
    Serial.print("wifi_mode: ");
    Serial.println(readResult);
    readResult = config->getPointer("wifi_ssid");
    Serial.print("wifi_ssid: ");
    Serial.println(readResult);
    readResult = config->getPointer("wifi_password");
    Serial.print("wifi_password: ");
    Serial.println(readResult);



    Serial.println("\n - start wifi with last parameters-");
    setupWiFi();    

    Serial.println("\n\n - END -");
    Serial.println();
}


void loop() {
    yield();
	server.handleClient();
	yield(); // Important!!
}



void setupWiFi() {
    char *mode, *ssid, *pass;
    
	server.stop();
	WiFi.disconnect();
    #ifndef ARDUINO_ARCH_ESP32
	    WiFi.setAutoConnect(true);
	#endif
	WiFi.setAutoReconnect(true);
	
    mode = config->getPointer("wifi_mode");
    ssid = config->getPointer("wifi_ssid");
    pass = config->getPointer("wifi_password");

    if (strcmp(mode, "S") == 0) {
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid, pass);
		// Wait for connection
		uint8_t i = 0;
		while (WiFi.status() != WL_CONNECTED && i++ < 30) { //wait 30*2 seconds
			Serial.print('.');
			delay(2000);
		}
		if (i == 31) {
			Serial.print("Could not connect to ");
			Serial.println(ssid);
			return;
		}
		Serial.print("Connected! IP address: ");
		Serial.println(WiFi.localIP());
	} else { // Default mode, 'A' (AP)
		WiFi.mode(WIFI_AP);
		WiFi.softAP(ssid, pass);
		Serial.print("SoftAP created! IP address: ");
		Serial.println(WiFi.softAPIP());
	}

	server.on("/configHtml", HTTP_GET, handleGetConfigHtml);
	server.on("/configJson", HTTP_GET, handleGetConfigJson);
	server.on("/uConfigLib/saveConfig", HTTP_POST, handleSaveConfig);
	server.onNotFound(handleDefault);
    yield();
	server.begin();
    yield();
	Serial.println("HTTP server started");
}

void handleGetConfigHtml() {
    Serial.println(" -> Requested configHTML");
    yield();
    config->handleConfigRequestHtml(&server);
}
void handleGetConfigJson() {
    Serial.println(" -> Requested configJSON");
    yield();
    config->handleConfigRequestJson(&server);
}
void handleSaveConfig() {
    Serial.println(" -> Requested saveConfig");
    yield();
    config->handleSaveConfig(&server);
}
void handleDefault() {
    Serial.println(" -> Requested default");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    yield();
    server.send(200, "text/html", "");
    yield();
    server.sendContent("<html><head><title>IoT device config - uConfigLib</title></head><body><p>Example. Available items:</p>");
    yield();

    server.sendContent("<p><a href=\"/configHtml\">HTML config page</a></p>");
    yield();
    server.sendContent("<p><a href=\"/configJson\">JSON config info</a></p>");
    yield();
    server.sendContent("</body>");
}


