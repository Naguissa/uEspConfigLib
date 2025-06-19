# The deffinitive ESP32 and ESP8266 configuration Arduino library, uEspConfigLib

Master status:  ![Arduino CI](https://github.com/Naguissa/uEspConfigLib/workflows/arduino_ci/badge.svg)

## What is this repository for?

This library consist in 2 parts:

 * One main library, uEspConfigLib, that manages configuration options, defaults, configuration site (HTML and JSON), configuration processing (from a POST of previous site) and configuration storage.
 
 * One interface to manage different configuration storages.
 
Currently storage classes are: SD-card, LittleFS, SPIFFS and none (values are lost after restart).


## Usage ##


To use it at any point on your sketch, define needed globals:

```
uEspConfigLibFSInterface * configFs;
uEspConfigLib * config;
```


Then, at setup(),  you need to choose an storage option among this:
```
configFs = new uEspConfigLibFSNone();
configFs = new uEspConfigLibFSSpiffs("/config.ini", bool initFS);
configFs = new uEspConfigLibFSLittlefs("/config.ini", bool initFS);
configFs = new uEspConfigLibFSSd("/config.ini", bool initFS, uint8_t CS_pin);
```

- If initFS option is true the class will initialize the filesystem as needed, even format it if still not done.
- Use CS_pin on SD card option to define CS pin when you use a non-standard one.

Still on setup, then you define your config object:

```
config = new uEspConfigLib(configFs);
```

Finally you need to define your desired configuration options:

```
config->addOption("name", "Description", "Default value");
```

A typical example could be:

```
config->addOption("wifi_mode", "WiFi mode (C=Client, other=Access Point)", "");
config->addOption("wifi_ssid", "SSID of your WiFi", "Unconfigured_device");
config->addOption("wifi_password", "Password of your WiFi", "wifi_password");
```

**Note:** When an option is defined inmediately its value is set to defined default value.



After that, you can start to use and manipulate any defined option.



**Read an option:**

```
char * charArrayPointer = config->getPointer("name");
```

Following previous example:

```
readResult = config->getPointer("wifi_mode");
Serial.print("wifi_mode: ");
Serial.println(readResult);
readResult = config->getPointer("wifi_ssid");
Serial.print("wifi_ssid: ");
Serial.println(readResult);
readResult = config->getPointer("wifi_password");
Serial.print("wifi_password: ");
Serial.println(readResult);
```

**Note:** If option "name" is not defined it will return a NULL pointer.



**Change an option:**


You can change any defined option using:

```
bool optionFound = config->set("name", "new_value");
```
    
Following previous example:
    
```
config->set("wifi_mode", "A");
config->set("wifi_ssid", "uEspConfigLib_EXAMPLE");
config->set("wifi_password", "");
```



This may be useful by itself, and this is why uEspConfigLibFSNone is available, but here's one huge extra: FS classes.


**Configuration file:**

This library uses a plain text file to store configuration options. File structure is:

```
# uConfigLib Configuration file
# Visit https://github.com/Naguissa/uEspConfigLib for details
# Syntax:
# variable = value
# (spaces and empty lines doesn't care)
# This file comments: lines starting with #, ; or //

# variable_name -- variable_description
# Default value: variable_default_value
variable_name = variable_value

# variable_name -- variable_description
# Default value: variable_default_value
variable_name = variable_value

# variable_name -- variable_description
# Default value: variable_default_value
variable_name = variable_value
```

**Note:** This file can be edited manually if you want, but saving values will recreate and overwrite the file-



**Load configuration from filesystem:**

```
bool readResult = config->loadConfigFile();
```

This will open configuration file, parse it and set any of the defined options contained in that file.


**Store configuration from filesystem:**

```
bool saveResult = config->saveConfigFile();
```


This will recreate and overwrite configuration file with current values.





**Web functionality, the last magic touch:**

If you are using ESP web server:

```
#ifdef ARDUINO_ARCH_ESP32
    WebServer server(80);
#else
    ESP8266WebServer server(80);
#endif
```


You can use the configuration-related web functionality:


**Generate and serve an HTML page with a configuration form:**


```
void handleGetConfigHtml() {
    config->handleConfigRequestHtml(&server, "optional_post_path");
}

//[...]

server.on("/config_path", HTTP_GET, handleGetConfigHtml);
```


This makes uEspConfigLib to serve a configuration form on "/config_path" route when accessed via browser.

The optional "optional_post_path" indicates the path where the form values will be posted on submit. Default value is "/uConfigLib/saveConfig"


**Generate and serve a JSON document with current configuration:**

```
void handleGetConfigJson() {
    config->handleConfigRequestJson(&server);
}

//[...]


server.on("/config_json_path", HTTP_GET, handleGetConfigJson);
```

This makes uEspConfigLib to serve a configuration JSON file with this format:

```
{
    "data": [
        {
            "name": "variable_name",
            "description": "variable_description",
            "defaultValue": "variable_default_value",
            "value": "variable_current_value"
        },
        {
            "name": "variable_name",
            "description": "variable_description",
            "defaultValue": "variable_default_value",
            "value": "variable_current_value"
        },
        {
            "name": "variable_name",
            "description": "variable_description",
            "defaultValue": "variable_default_value",
            "value": "variable_current_value"
        }
    ]
}
```

This is useful if you have a complete webpage, as you can add your look&feel to the configuration system.




**Process and store configuration:**

```
void handleSaveConfig() {
    config->handleSaveConfig(&server);
}

//[...]

server.on("/save_config_path", HTTP_POST, handleSaveConfig);
```

This will process and store any defined value passed as argument. This accepts any number of valid values, from one to all.

If a configuration option is not defined as argument it will skip it. But if the argument is defined but empty it will store empty string as new value.


There's an special argument: if "format" is defined as "json" this call will respond a JSON document:

```
{
    "data": {
        "result": 1
    }
}
```

Otherwise it will respond redirecting using 302 redirect to "/?saved=1"

This way it can be used both for simple HTML requests and for JSON requests.


**Note:** This function also calls saveConfigFile(), so it's not needed to do so manually.




## How do I get set up?

You can get it from Arduino libraries directly, searching by uEspConfigLib.

For manual installation:

 * Get the ZIP from releases link: https://github.com/Naguissa/uEspConfigLib/releases
 * Rename to uEspConfigLib.zip
 * Install library on Arduino


## Documentation

You can find all documentation in this repository: https://github.com/Naguissa/uEspConfigLib_doc_and_extras

You can read documentation online here: https://naguissa.github.io/uEspConfigLib_doc_and_extras/



## Example

Included on examples folder, available on Arduino IDE. The example shows example code in this file but with a fully working sketch and adds extra use cases.

See [uEspConfigLib_example.ino](examples/uEspConfigLib_example/uEspConfigLib_example.ino)


## Who do I talk to?

 * [Naguissa](https://github.com/Naguissa)
 * https://www.foroelectro.net


## Contribute

Any code contribution, report or comment are always welcome. Don't hesitate to use GitHub for that.


 * You can sponsor this project using GitHub's Sponsor button: https://github.com/Naguissa/uEspConfigLib
 * You can make a donation via PayPal: https://paypal.me/foroelectro


Thanks for your support.


Contributors hall of fame: https://www.foroelectro.net/hall-of-fame-f32/contributors-contribuyentes-t271.html
