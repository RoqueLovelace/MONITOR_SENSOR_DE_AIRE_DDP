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

struct settings
{
  char ssid[30];
  char password[30];
  char endpoint[100];
  int requestIntervalTime;
} user_wifi = {};

LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C
unsigned long previousMillis = 0;
const int intervaloEntreRequests = 10;
long intervaloContador = 1000;
int tiempoRestante = intervaloEntreRequests;

String serverURL = "http://192.168.3.6:1234/post";

const char *ntpServer = "pool.ntp.org"; // Servidor NTP
const long gmtOffset_sec = -21600;      // Ajuste de zona horaria (GMT-6)
const int daylightOffset_sec = 0;       // Ajuste por horario de verano

char timeStringBuff[50] = "0-0-0";
char dateBuff[11]; // YYYY-MM-DD (10 caracteres + 1 para '\0')
char timeBuff[9];  // HH:MM:SS (8 caracteres + 1 para '\0')
String processOutput[4] = {"", "", "", ""};
int contadorProcessOutput = 0;

void setup()
{
  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, user_wifi);

  pinMode(ledRojo, OUTPUT);
  pinMode(ledAmarillo, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);

  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    if (tries++ > 30)
    {
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

  if (WiFi.status() == WL_CONNECTED)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CONECTADO, IP:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // Configuramos NTP
    delay(5000);

    EnviarRequest(); // Enviamos datos al servidor
  }
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    server.handleClient();
  }
  else
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= intervaloContador)
    {
      previousMillis = currentMillis;

      if (tiempoRestante > 0)
      {
        tiempoRestante--;
        lcd.clear();

        // Mostramos el PPM y el tiempo restante
        lcd.setCursor(0, 0);
        int ppm = analogRead(sensorData);
        lcd.print("ppm:");
        lcd.print(ppm);

        // Formateamos el tiempo
        char tiempoFormato[6];
        sprintf(tiempoFormato, "%02d:%02d", tiempoRestante / 60, tiempoRestante % 60);
        lcd.setCursor(11, 0);
        lcd.print(tiempoFormato);

        // Intercalamos la información de la última petición
        lcd.setCursor(0, 1);
        lcd.print(processOutput[contadorProcessOutput].substring(0, 16));
        contadorProcessOutput = (contadorProcessOutput + 1) % 4;

        // Control de LEDs según los valores de PPM
        if (ppm == -1 || ppm >= 1000)
        {
          digitalWrite(ledVerde, LOW);
          digitalWrite(ledAmarillo, LOW);
          digitalWrite(ledRojo, HIGH);
        }
        else if (ppm >= 400)
        {
          digitalWrite(ledVerde, LOW);
          digitalWrite(ledAmarillo, HIGH);
          digitalWrite(ledRojo, LOW);
        }
        else
        {
          digitalWrite(ledVerde, HIGH);
          digitalWrite(ledAmarillo, LOW);
          digitalWrite(ledRojo, LOW);
        }
      }
      else
      {
        EnviarRequest();
        tiempoRestante = user_wifi.requestIntervalTime; // Reiniciar el temporizador
      }
    }
  }
}

void handlePortal()
{
  if (server.method() == HTTP_POST)
  {
    strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
    strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
    strncpy(user_wifi.endpoint, server.arg("endpoint").c_str(), sizeof(user_wifi.endpoint));
    user_wifi.requestIntervalTime = server.arg("intervalo").toInt(); // Convertir a entero
    EEPROM.put(0, user_wifi);
    EEPROM.commit();

    server.send(200, "text/html", "<html><body><h1>WiFi Saved. Restart the device.</h1></body></html>");
  }
  else
  {
    server.send(200, "text/html",
                "<html>"
                "<head>"
                "<style>"
                "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; padding-top: 50px; }"
                "form { background-color: #ffffff; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); display: inline-block; }"
                "input[type='text'], input[type='password'] { width: 80%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 4px; font-size: 16px; }"
                "input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; }"
                "input[type='submit']:hover { background-color: #45a049; }"
                "</style>"
                "</head>"
                "<body>"
                "<form method='POST'>"
                "<h2>WiFi Configuration</h2>"
                "<input type='text' name='ssid' placeholder='SSID'><br>"
                "<input type='password' name='password' placeholder='Password'><br>"
                "<input type='text' name='endpoint' placeholder='URL endpoint'><br>"
                "<input type='text' name='intervalo' placeholder='Segundos entre requests'><br>"
                "<input type='submit' value='Guardar Configuración'>"
                "</form>"
                "</body>"
                "</html>");
  }
}

void EnviarRequest()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    WiFiClient client;

    // Obtenemos fecha y hora
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(dateBuff, sizeof(dateBuff), "%Y-%m-%d", &timeinfo);
    strftime(timeBuff, sizeof(timeBuff), "%H:%M:%S", &timeinfo);

    // Construimos el JSON con los datos del sensor
    int ppm = analogRead(sensorData);
    String jsonPayload = "{\"ppm\": " + String(ppm) + ", \"date\": \"" + String(dateBuff) + "\", \"time\": \"" + String(timeBuff) + "\"}";

    // Enviamos la petición HTTP
    http.begin(client, user_wifi.endpoint);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonPayload);

    // Procesamos la respuesta
    if (httpResponseCode > 0)
    {
      String response = http.getString();
      processOutput[0] = "Req. SENDED";
      processOutput[1] = String(timeBuff);
      processOutput[2] = "Code: " + String(httpResponseCode);
      processOutput[3] = "Resp: " + response;
    }
    else
    {
      processOutput[0] = "ERROR IN REQUEST";
      processOutput[1] = "Failed";
      processOutput[2] = "No Connection";
    }
    http.end();
  }
}
