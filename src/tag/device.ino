#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "../lib/DW1000/DW1000Ranging.h"
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

  server.on("/api/",getAPIAll);
  server.on("/api/name",getAPIName);

  server.begin();
  
  Serial.println("Initialization Completed.");
}
void loop() {
  DW1000Ranging.loop();
  server.handleClient();
}

//================== DWB1000 Controls =====================

void newRange()
{
    Serial.print("from: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
    Serial.print("\t Range: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    Serial.println(" dBm");
}

void newDevice(DW1000Device *device)
{
    Serial.print("ranging init; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
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

  appendToJson("Id", ID);
  appendToJson("Name", getFullName());
  appendToJson("Age", AGE);
  appendToJson("EUI", ADDR_TAG_1);

  serializeJson(jsonDocument, jsonBuffer);
  server.send(200, "application/json", jsonBuffer); 
}

void getAPIName() {
  jsonDocument.clear();

  jsonDocument["tag"] = "Name";
  jsonDocument["val"] = getFullName();

  serializeJson(jsonDocument, jsonBuffer);  
  server.send(200, "application/json", jsonBuffer);
}

//=============== Auxiliary Functions ====================

void appendToJson(String tag, String value) {
  JsonObject item = jsonDocument.createNestedObject();
  item["tag"] = tag;
  item["val"] = value;
}

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