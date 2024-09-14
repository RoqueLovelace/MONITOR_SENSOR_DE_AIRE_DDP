#include <time.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>

#define ledVerde 14
#define ledAmarillo 12
#define ledRojo 13
#define sensorData A0

// region VARIABLES DE CONTADOR
unsigned long previousMillis = 0;    // Guarda el último valor de millis()
const int intervaloEntreRequests = 10;
const long interval = 1000;          // Intervalo de tiempo (1 segundo = 1000 ms)
int tiempoRestante = intervaloEntreRequests;            // Tiempo del temporizador en segundos (10 minutos = 600 segundos)
// endregion

// region VARIABLES HOTA CON SERVER NTP
const char* ntpServer = "pool.ntp.org";  // Servidor NTP
const long  gmtOffset_sec = -21600;      // Ajuste de zona horaria (para GMT-6, hora local)
const int   daylightOffset_sec = 0;      // Ajuste por horario de verano
int lastRequest = 0;
// endregion

// region WIFI 
const char* ssid = "";
const char* password = "";
// endregion

// region LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C
// endregion

// region SERVER
String serverURL = "http://ip:1234/post";
// endregion

void setup() {
  // Inicializar LEDs como salida
  pinMode(ledRojo, OUTPUT);  
  pinMode(ledAmarillo, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  // inicializamos lcd
  lcd.init();
  lcd.backlight();  
  lcd.setCursor(0, 0);

  // Inicializar puerto serie
  Serial.begin(9600);
}

void loop() {
  // Obtener el tiempo actual
  unsigned long currentMillis = millis();

  // Verificar si ha pasado 1 segundo
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Guardar el tiempo actual para la próxima comparación
    
    if (tiempoRestante > 0) {
      // Restar un segundo al temporizador
      tiempoRestante--;

      // Mostrar el tiempo restante en el LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tiempo: ");
      lcd.print(tiempoRestante / 60);  // Minutos restantes
      lcd.print(":");
      lcd.print(tiempoRestante % 60);  // Segundos restantes
      
      // También mostrar el tiempo restante en el puerto serie
      Serial.print("Tiempo restante: ");
      Serial.print(tiempoRestante / 60);
      Serial.print(" minutos, ");
      Serial.print(tiempoRestante % 60);
      Serial.println(" segundos");
    } else {
      // Cuando el tiempo llega a 0
      Serial.println("¡El tiempo ha terminado!");

      // Acción adicional: encender LED rojo
      digitalWrite(ledVerde, HIGH);
      delay(1000); // Mantener LED encendido 1 segundo
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledRojo, HIGH);
      delay(1000); // Mantener LED encendido 1 segundo
      digitalWrite(ledRojo, LOW);
      digitalWrite(ledAmarillo, HIGH);
      delay(1000); // Mantener LED encendido 1 segundo
      digitalWrite(ledAmarillo, LOW);

      // Reiniciar el temporizador a 10 minutos (600 segundos)
      tiempoRestante = intervaloEntreRequests;

      // También podrías realizar cualquier otra acción aquí, como sonar una alarma, etc.
    }
  }
}


void probarLeds() {
  digitalWrite(ledRojo, HIGH);  // Turn the LED on (Note that LOW is the voltage level
  digitalWrite(ledAmarillo, LOW);
  digitalWrite(ledVerde, LOW);
  delay(1000);

  digitalWrite(ledRojo, LOW);  
  digitalWrite(ledAmarillo, HIGH);
  digitalWrite(ledVerde, LOW);
  delay(1000);

  digitalWrite(ledRojo, LOW);  
  digitalWrite(ledAmarillo, LOW);
  digitalWrite(ledVerde, HIGH);
  delay(1000);
}

