#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "../lib/DW1000/DW1000Ranging.h"    //non-Arduino IDE
//#include <DW1000Ranging.h>                //If modified to base library directly
#include "config.h"
#include "profile.h"

#define DEV_ADDR ADDR_TAG_1
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;
const uint8_t PIN_SS = 4;

const char* ssid = SERV_TAG_SSID;
const char* pass = SERV_TAG_PWD;

IPAddress local(IP_ADDR[0],IP_ADDR[1],IP_ADDR[2],IP_ADDR[3]);
IPAddress gateway(IP_GATEWAY[0],IP_GATEWAY[1],IP_GATEWAY[2],IP_GATEWAY[3]);
IPAddress subnet(IP_SUBNET[0],IP_SUBNET[1],IP_SUBNET[2],IP_SUBNET[3]);
WebServer server(PORT);

StaticJsonDocument<250> jsonDocument;
char jsonBuffer[250];
String pageBuffer = "";

struct Anchor{
    bool isActive = false;
    uint16_t address;
    float range;
    float rx_power;
} currentDevices[MAX_ANCHOR];

//========================= Main ==========================

void setup() {
    Serial.begin(115200);
    delay(1000);

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);

    //DW1000Ranging.useRangeFilter(true);

    DW1000Ranging.startAsTag(DEV_ADDR, DW1000.MODE_LONGDATA_RANGE_LOWPOWER);

    WiFi.softAP(ssid, pass);
    WiFi.softAPConfig(local, gateway, subnet);
    delay(100);
    
    buildPageInfo();
    server.on("/",getPageInfo);
    server.onNotFound(getPageError);

    server.on(API_ALL_LOC,getAPIAll);
    server.on(API_NAME_LOC,getAPIName);

    server.begin();
    
    Serial.println("Initialization Completed.");
}
void loop() {
    DW1000Ranging.loop();
    server.handleClient();
}

//================== DWM1000 Controls =====================
void newRange()
{
    uint16_t address = DW1000Ranging.getDistantDevice()->getShortAddress();
    float range = DW1000Ranging.getDistantDevice()->getRange();
    float rx_power = DW1000Ranging.getDistantDevice()->getRXPower();

    for(int i=0;i < MAX_ANCHOR;i++){
        if(!currentDevices[i].isActive||currentDevices[i].address!=address){continue;}
        currentDevices[i].range = range;
        currentDevices[i].rx_power = rx_power;
        break;
    }

    #ifdef DEBUG_MODE
    Serial.print("from: ");
    Serial.print(address, HEX);
    Serial.print("\t Range: ");
    Serial.print(range);
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(rx_power);
    Serial.println(" dBm");
    #endif
}

void newDevice(DW1000Device *device)
{
    uint16_t address = device->getShortAddress();
    for(int i=0;i<MAX_ANCHOR;i++){
        if(currentDevices[i].isActive){continue;}
        currentDevices[i].address = address;
        currentDevices[i].isActive = true;
        break;
    }

    #ifdef DEBUG_MODE
    Serial.print("ranging init; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(address, HEX);
    #endif
}

void inactiveDevice(DW1000Device *device)
{
    uint16_t address = device->getShortAddress();
    for(int i=0;i<MAX_ANCHOR;i++){
        if(currentDevices[i].address!=address){continue;}
        currentDevices[i].isActive = false;
        break;
    }

    #ifdef DEBUG_MODE
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
    #endif
}

//================== Page Redirections ====================

void getPageInfo() {
    server.send(200, "text/html", pageBuffer); 
}

void getPageError(){
    server.send(404, "text/plain", "Error 404: Page not found");
}

//================== API Redirections ====================

void getAPIAll() {
    jsonDocument.clear();

    jsonDocument["EUI"] = ADDR_TAG_1;
    jsonDocument["ID"] = ID;
    jsonDocument["Name"] = getFullName();
    jsonDocument["Age"] = AGE;
    
    JsonArray anchorList = jsonDocument.createNestedArray("Anchor");
    for(int i =0;i < MAX_ANCHOR;i++){
        if(!currentDevices[i].isActive){continue;}
        JsonObject anchorDevice = anchorList.createNestedObject();
        anchorDevice["EUI"] = String(currentDevices[i].address);
        anchorDevice["Range"] = String(currentDevices[i].range);
        anchorDevice["Power"] = String(currentDevices[i].rx_power);
    }

    serializeJson(jsonDocument, jsonBuffer);
    server.send(200, "application/json", jsonBuffer); 
}

void getAPIName() {
    jsonDocument.clear();
    jsonDocument["Name"] = getFullName();
    serializeJson(jsonDocument, jsonBuffer);  
    server.send(200, "application/json", jsonBuffer);
}

//=============== Auxiliary Functions ====================

String getFullName(){
    String name = FNAME;
    name.concat(" ");
    name.concat(LNAME);
    return name;
}

//==================== HTML Page =======================

void buildPageInfo(){
    pageBuffer +="<!DOCTYPE html> <html>\n";
    pageBuffer +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    pageBuffer +="<title>Info Page</title>\n";
    pageBuffer +="</head>\n";
    pageBuffer +="<body>\n";
    
    pageBuffer +="<h5>ID: ";
    pageBuffer += ID;
    pageBuffer += "</h5>\n";

    pageBuffer +="<h1>";
    pageBuffer += getFullName();
    pageBuffer += "</h1>\n";

    pageBuffer +="<h3>Age: ";
    pageBuffer += AGE;
    pageBuffer += "</h3>\n";

    pageBuffer +="</body>\n";
    pageBuffer +="</html>\n";
}