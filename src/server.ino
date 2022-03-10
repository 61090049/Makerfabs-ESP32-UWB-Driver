#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config_tag.h"
#include "profile.h"

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
  server.handleClient();
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
  pageBuffer +="</h1>\n";

  pageBuffer +="<h3>Age: ";
  pageBuffer += AGE;
  pageBuffer += "</h3>\n";

  pageBuffer +="</body>\n";
  pageBuffer +="</html>\n";
}