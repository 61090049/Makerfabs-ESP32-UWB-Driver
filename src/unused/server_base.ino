#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "user";
const char* pass = "123";

IPAddress local(10,0,0,1);
IPAddress gateway(10,0,0,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

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

  appendToJson("Id", "00000");
  appendToJson("Name", getFullName());
  appendToJson("Age", "Age");
  appendToJson("EUI", "00:00:00:00:00:00:00:00");

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
  String name = "FName";
  name.concat(" ");
  name.concat("LName");
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
  pageBuffer +="00000";
  pageBuffer +="</h5>\n";

  pageBuffer +="<h1>";
  pageBuffer += getFullName();
  pageBuffer +="</h1>\n";

  pageBuffer +="<h3>Age: ";
  pageBuffer +="Age";
  pageBuffer +="</h3>\n";

  pageBuffer +="</body>\n";
  pageBuffer +="</html>\n";
}