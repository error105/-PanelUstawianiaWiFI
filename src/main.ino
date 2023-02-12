#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

// Dane APka, pod jakim bedzie sie rozglaszal nasz ESP.
String ssidbac = "AP-01";
String passbac = "12345678";

// Zmienne do odczytu danych WiFi z pliku tekstowego
String ssid, password;
// Nazwa pliku gdzie trzymamy poswiadczenia
const char* fsFilename = "/wifi.txt";
// Serwer www
ESP8266WebServer server(80);


void setup() {
  Serial.begin(115200);
  // Ponizszy wpis odczytuje dane WiFi z pliku wifi.txt
  readWifiCredentials();
  Serial.println();
  Serial.println("_________________");
  Serial.println();
  
  // Staramy sie nawiazac polaczenie, i tworzymy zmienna lokalna counter - do zliczania ile razy juz sie laczylismy
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int counter = 0;
  
  while (WiFi.status() != WL_CONNECTED && counter < 40) {
    // Ustawiamy ile czasu mamy poswiecic na polaczenie z WiFi (250ms * 40)
    delay(250);
    counter++;
    }
    if (WiFi.status() != WL_CONNECTED) {
      // Jesli przekroczylismy czas polaczenia, tworzymy AP
      Serial.println("Tworzenie AP...");
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssidbac, passbac);
      // I odpalamy serwer na ktorym wklepiemy nasze nowe dane
      startWebServer();
    } else {
      // Poalczenie sie powiodlo, wydrukukujmy IP
      Serial.println("Polaczono z WiFi");
      Serial.println("Adres IP: ");
      Serial.println(WiFi.localIP());
    }
}

void loop() {
  // Utrzymywanie serwera WWW przy zyciu jesli nie udalosie polaczyc z Siecia WiFi
  if (WiFi.status() != WL_CONNECTED) {
    server.handleClient();}
  // Dalej wstawiamy juz nasz kod

}

// Odczytywanie danych z pliku.
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

// Zapisywanie danych w pliku i restart ESP
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

// Strona WWW na ktorej mozemy wprowadzic nasze nowe poswiadczenia
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

// Wiersz odpowiedzialny za start strony WWW
void startWebServer() {
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}