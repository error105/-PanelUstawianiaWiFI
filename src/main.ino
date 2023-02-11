#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

String ssidbac = "LicznikOdwiedzin";
String passbac = "1qaz@WSX";
String ssid, password;

const char* fsFilename = "/wifi.txt";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  readWifiCredentials();
  Serial.println();
  Serial.println("_________________");
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int counter = 0;
  
  while (WiFi.status() != WL_CONNECTED && counter < 40) {
    delay(250);
    counter++;
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Creating AP...");
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssidbac, passbac);
      startWebServer();
    } else {
      Serial.println("Connected to WiFi");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      startWebServer();
    }
}

void loop() {
  server.handleClient();
}

void readWifiCredentials() {
  if (LittleFS.begin()) {
    if (LittleFS.exists(fsFilename)) {
      File file = LittleFS.open(fsFilename, "r");
      if (file) {
        ssid = file.readStringUntil('\n').c_str();
        ssid.trim();
        password = file.readStringUntil('\n').c_str();
        password.trim();
        file.close();
      }
    }
  }
}

void writeWifiCredentials(String ssid, String password) {
  if (LittleFS.begin()) {
    LittleFS.format();
    File file = LittleFS.open(fsFilename, "w");
    file.println(ssid);
    file.print(password);
    file.close();
    ESP.restart();
  }
}

void handleRoot() {
  String html = "<html><body><center>";
  html += "<h1>Ustawienia WiFi i API</h1>";
  html += "<form action='/set' method='post'>";
  html += "SSID: <input type='text' name='ssid' value='" + String(ssid) + "'><br>";
  html += "PASSWORD: <input type='text' name='password' value='" + String(password) + "'><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form>";
  html += "</center></body></html>";
  server.send(200, "text/html", html);
}

void handleSet() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    writeWifiCredentials(server.arg("ssid"), server.arg("password"));
  }
}

void startWebServer() {
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}