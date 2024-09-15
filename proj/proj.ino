#include <time.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>

#define ledVerde 14
#define ledAmarillo 12
#define ledRojo 13
#define sensorData A0

#pragma region VARIABLES DE CONTADOR
unsigned long previousMillis = 0; // Guarda el último valor de millis()
const int intervaloEntreRequests = 5;
long intervaloContador = 1000;               // Intervalo de tiempo (1 segundo = 1000 ms)
int tiempoRestante = intervaloEntreRequests; // Tiempo del temporizador en segundos (10 minutos = 600 segundos)
#pragma endregion

#pragma region VARIABLES HOTA CON SERVER NTP
const char *ntpServer = "pool.ntp.org"; // Servidor NTP
const long gmtOffset_sec = -21600;      // Ajuste de zona horaria (para GMT-6, hora local)
const int daylightOffset_sec = 0;       // Ajuste por horario de verano
#pragma endregion

#pragma region WIFI
const char *ssid = ";";
const char *password = "";
#pragma endregion

#pragma region LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C
int charPosicion = 0;
#pragma endregion

#pragma region SERVER
String serverURL = "http://ipipip:1234/post";
#pragma endregion

#pragma region VARIABLES DE HTTPRequest
int ppm = -1; // se escoge -1 por si hay algun fallo, se podra ver este valor irreal
String jsonPayload = "";
char timeStringBuff[50] = "0-0-0";
int httpResponseCode = -1;
String response = "";
bool lastRequestFailed4Connection = false;
bool lastRequestFailed = false;
char tiempoFormato[6]; // Buffer para almacenar el tiempo formateado
#pragma endregion

#pragma region STRINGS
// tiempo, LastReqSuccesfull,  response, responseStatus
// si no hay conexion: CHECK CONNECTION (4)
String processOutput[4] = {"", "", "", ""};
int contadorProcessOutput = 0;
#pragma endregion

void setup()
{

#pragma region Inicializar LEDs como salida
  pinMode(ledRojo, OUTPUT);
  pinMode(ledAmarillo, OUTPUT);
  pinMode(ledVerde, OUTPUT);
#pragma endregion
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarillo, LOW);
  digitalWrite(ledRojo, LOW);
  delay(500);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarillo, HIGH);
  digitalWrite(ledRojo, LOW);
  delay(500);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarillo, LOW);
  digitalWrite(ledRojo, HIGH);
  delay(500);

#pragma region inicializamos lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
#pragma endregion

#pragma region Inicializamos WIFI
  WiFi.begin(ssid, password);
  lcd.print("Conectando... ");
  lcd.setCursor(0, 1);
#pragma endregion

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledAmarillo, LOW);
    digitalWrite(ledRojo, LOW);
    delay(166);
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarillo, HIGH);
    digitalWrite(ledRojo, LOW);
    delay(166);
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarillo, LOW);
    digitalWrite(ledRojo, HIGH);
    delay(166);
    lcd.print(".");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONECTADO, IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // configuramos NTP
  delay(5000);

  ppm = analogRead(sensorData); // se vincula el ppm leido
  EnviarRequest();
}

void loop()
{
  // se obtiene el tiempo actual
  unsigned long currentMillis = millis();

  // se verifica si ha pasado 1 segundo
  if (currentMillis - previousMillis >= intervaloContador)
  {
    previousMillis = currentMillis; // Guardar el tiempo actual para la próxima comparación

    if (tiempoRestante > 0)
    {

      // se resta un segundo al temporizador
      tiempoRestante--;

      // se limpia la pantalla
      lcd.clear();

#pragma region IMPRIMIR PRIMER LINEA
      // en este caso, tiene que irse mostrando informacion
      // arriba ppm y tiempo
      // 0123456789012345
      // ppm:00000--00:00
      //----------------

      // Mostrar los ultimos ppm registrados
      lcd.setCursor(0, 0);
      lcd.print("ppm:");
      lcd.print(ppm);

      // Mostrar el tiempo restante en el LCD
      sprintf(tiempoFormato, "%02d:%02d", tiempoRestante / 60, tiempoRestante % 60); // se formatea el tiempo
      lcd.setCursor(11, 0);
      lcd.print(tiempoFormato);
#pragma endregion

#pragma region IMPRIMIR SEGUNDA LINEA
      // abajo intercalando entre: si hay conexion, hora de la ultima peticion, response de la ultima, status code
      lcd.setCursor(0, 1);
      lcd.print(processOutput[contadorProcessOutput].substring(0, 16));
      if (contadorProcessOutput == 3)
        contadorProcessOutput = 0;
      else
        contadorProcessOutput++;
#pragma endregion

#pragma region LEDs
      if (ppm == -1 || ppm >= 1000)
      {
        // PPM no válido o muy alto
        digitalWrite(ledVerde, LOW);
        digitalWrite(ledAmarillo, LOW);
        digitalWrite(ledRojo, HIGH);
      }
      else if (ppm >= 400)
      {
        // PPM en rango crítico
        digitalWrite(ledVerde, LOW);
        digitalWrite(ledAmarillo, HIGH);
        digitalWrite(ledRojo, LOW);
      }
      else
      {
        // PPM en rango normal
        digitalWrite(ledVerde, HIGH);
        digitalWrite(ledAmarillo, LOW);
        digitalWrite(ledRojo, LOW);
      }
#pragma endregion
    }
    else
    {                               // Cuando el tiempo llega a 0
      ppm = analogRead(sensorData); // se vincula el ppm leido
      EnviarRequest();
      tiempoRestante = intervaloEntreRequests; // Reiniciar el temporizador
    }
  }
}

void EnviarRequest()
{
  if (WiFi.status() == WL_CONNECTED)
  { // si no se ha roto la conexion a internet
#pragma region VARIABLES DE CONECTION
    HTTPClient http;
    WiFiClient client;
#pragma endregion

#pragma region TIMESTAMP
    // obtenemos la hora fecha y hora actual con un servidor NTP
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo); // se vincula la hora formateada en el buffer timeStringBuff
#pragma endregion

#pragma region presentacion de datos
    jsonPayload = "{\"ppm\": " + String(ppm) + ", \"timestamp\": \"" + String(timeStringBuff) + "\"}";

    strftime(timeStringBuff, sizeof(timeStringBuff), "%d-%m-%y %H:%M", &timeinfo); // modificamos la hora para ahorar espacio en el lcd
#pragma endregion

#pragma region ARMADO Y ENVIO DE Request
    http.begin(client, serverURL);
    http.addHeader("Content-Type", "application/json");

    httpResponseCode = http.POST(jsonPayload); // se hace el Request y se recupera el status para proceder
#pragma endregion

#pragma region PROCEDIMIENTO POSTERIOR A ENVIO DE Request
    if (httpResponseCode > 0)
    { // si el httpResponseCode se envio, necesitamos saber si fue correcto o no
      response = http.getString();
      // tiempo, LastReqSuccesfull,  response, responseStatus
      processOutput[0] = "Req. SENDED";
      processOutput[1] = timeStringBuff;
      processOutput[2] = "Req. CODE: " + String(httpResponseCode);
      processOutput[3] = "Req.INFO:" + response;
    }
    else
    {
      processOutput[0] = "ERROR IN REQUEST";
      processOutput[1] = "REQ. FAILED";
      processOutput[2] = "SERVER ERROR";
      processOutput[3] = String("LAST REQ. ") + timeStringBuff;
    }
#pragma endregion
  }
  else
  { // si la conexion se rompio
    // TODO: EVALUAR SI SE DEBERIA DE BORRAR INFO DE DATOS SACADOS DEL REQUEST, YA QUE NO SON VALIDOS
    // lastRequestFailed = true;
    // lastRequestFailed4Connection = true;
    lcd.print("NO CONNECTION");
    lcd.setCursor(0, 2);
    lcd.print("Request failed");
    processOutput[0] = "CHECK CONNECTION";
    processOutput[1] = "NO REQ. SENDED";
    processOutput[2] = "CHECK CONNECTION";
    processOutput[3] = String("LAST REQ. ") + timeStringBuff;
  }
}