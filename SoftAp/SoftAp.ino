#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <time.h>

#define ledVerde 14
#define ledAmarillo 12
#define ledRojo 13
#define sensorData A0

ESP8266WebServer server(80);

struct settings {
  char ssid[30];
  char password[30];
} user_wifi = {};

LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C
unsigned long previousMillis = 0;
const int intervaloEntreRequests = 300;
long intervaloContador = 1000;
int tiempoRestante = intervaloEntreRequests;

String serverURL = "http://192.168.3.6:1234/post";

void setup() {
  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, user_wifi);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);

  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (tries++ > 30) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP("Setup Sensor", "DDPSENSOR.01L");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Setup AP Mode");
      server.on("/", handlePortal);
      server.begin();
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CONECTADO, IP:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    
    configTime(-21600, 0, "pool.ntp.org");
    delay(5000);

    EnviarRequest();  // Función para enviar datos del sensor al servidor
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    server.handleClient();
  } else {
    // Código para manejar el envío de datos periódicos al servidor
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= intervaloContador) {
      previousMillis = currentMillis;
      tiempoRestante--;
      if (tiempoRestante <= 0) {
        EnviarRequest();
        tiempoRestante = intervaloEntreRequests;
      }
    }
  }
}

void handlePortal() {
  if (server.method() == HTTP_POST) {
    strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
    strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
    EEPROM.put(0, user_wifi);
    EEPROM.commit();

    server.send(200, "text/html", "<html><body><h1>WiFi Saved. Restart the device.</h1></body></html>");
  } else {
    server.send(200, "text/html", "<html><body><form method='POST'><input name='ssid' placeholder='SSID'><input name='password' placeholder='Password' type='password'><input type='submit'></form></body></html>");
  }
}

void EnviarRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char dateBuff[11], timeBuff[9];
    strftime(dateBuff, sizeof(dateBuff), "%Y-%m-%d", &timeinfo);
    strftime(timeBuff, sizeof(timeBuff), "%H:%M:%S", &timeinfo);

    int ppm = analogRead(sensorData);
    String jsonPayload = "{\"ppm\": " + String(ppm) + ", \"date\": \"" + String(dateBuff) + "\", \"time\": \"" + String(timeBuff) + "\"}";

    http.begin(client, serverURL);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Request Sent!");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Request Failed!");
    }
    http.end();
  }
}
