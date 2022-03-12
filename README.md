# Makerfabs ESP32 UWB Driver

## Requirements 
A minimum of two units of UWB Development Board from Makerfabs are required. One will act as an anchoring device and the other as a tagging device.
Standard arduino libraries such as *SPI.h*, *Wifi.h*, and *WebServer.h* are utilize in the application.
Additional libraries used are *ArduinoJson* and a modified version of *DWM1000* library, the latter which is included with the repository.

## Anchoring Device
Settings are available in *config.h* :
```C++
#define ADDR_ANCHOR_1   "83:17:5B:D5:A9:9A:E2:9C"
#define ADDR_ANCHOR_2   "84:17:5B:D5:A9:9A:E2:9C"

#define DEBUG_MODE
```

## Tag Device
Settings are available in *config.h* :
```C++
#define ADDR_TAG_1      "7D:00:22:EA:82:60:3B:9C"

#define SERV_TAG_SSID   "UWB_Tag_1"
#define SERV_TAG_PWD    "123"

static const int IP_ADDR[4] = {192,168,80,1};
static const int IP_GATEWAY[4] = {192,168,80,1};
static const int IP_SUBNET[4] = {255,255,255,0};
#define PORT 80

#define API_ALL_LOC "/api/"
#define API_NAME_LOC "/api/name"

#define MAX_ANCHOR 4

#define DEBUG_MODE
```

The user's information is stored in *profile.h* :
```C++
#define ID "000001"
#define FNAME "John"
#define LNAME "Smith"
#define AGE "40"
```
## Accessing
The tag will act as a web server hosting both the information of the user and the location. Currently the tag is implemented in **Soft AP Mode** only.
To access the information, connects via a client device with wireless capability to the location configure in the tag's *config.h*. Data will be display on a html page or retrieved as json via api call.
