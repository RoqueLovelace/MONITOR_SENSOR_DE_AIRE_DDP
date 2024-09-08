#include <time.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>

#define ledRojo 14
#define ledAmarillo 12
#define ledVerde 13
#define sensorData A0

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C

const char* ntpServer = "pool.ntp.org";  // Servidor NTP
const long  gmtOffset_sec = -21600;      // Ajuste de zona horaria (para GMT-6, hora local)
const int   daylightOffset_sec = 0;      // Ajuste por horario de verano
int lastRequest = 0;

//para ingresar a la red
const char* ssid = "";
const char* password = "";

String serverURL = "http://ip:1234/post"; 

void setup() {

  // inicializamos lcd
  lcd.init();
  lcd.backlight();  
  lcd.setCursor(0, 0);

  // inicializamos leds como output
  pinMode(ledRojo, OUTPUT);  
  pinMode(ledAmarillo, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  //Inicializamos WIFI
  WiFi.begin(ssid, password);
  lcd.print("Conectando... ");
  lcd.setCursor(0,1);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("conectado, ip:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(5000);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // configuramos NTP
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    int ppm = analogRead(sensorData); 

    // obtenemos la hora fecha y hora actual con un servidor NTP
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Formateamos el timestamp en formato legible
    char timeStringBuff[50];  // Buffer para almacenar la cadena de tiempo
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);

    // Creamos el JSON con el valor del sensor y el timestamp formateado
    String jsonPayload = "{\"ppm\": " + String(ppm) + ", \"timestamp\": \"" + String(timeStringBuff) + "\"}";

    // modificamos la hora para ahorar espacio en el lcd
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M", &timeinfo);
    String nextRequest; //se calculara cuanto tiempo queda para la siguiente request.

    http.begin(client, serverURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonPayload);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ppm:");
    lcd.print(ppm);
    

    if (httpResponseCode > 0) {
      String response = http.getString();

      lcd.setCursor(0,1);
      lcd.print(timeStringBuff);  // Mostrar el tiempo formateado en la pantalla
      delay(1000);
      lcd.setCursor(0,1);
      lcd.print("Res.code: " + String(httpResponseCode));
      delay(1000);
    } else {
      lcd.setCursor(0,1);
      lcd.print("ErrorInHTTPRequest");
      delay(1000);
    }
    http.end();
  } else {
    lcd.setCursor(0,1);
    lcd.print("WiFi not connected");
    delay(1000);
  }
}