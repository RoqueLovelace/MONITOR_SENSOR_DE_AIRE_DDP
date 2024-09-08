#include <LiquidCrystal_I2C.h>

#define ledRojo 14
#define ledAmarillo 12
#define ledVerde 13
#define sensorData A0

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C

void setup() {
  // inicializar leds como output
  pinMode(ledRojo, OUTPUT);  
  pinMode(ledAmarillo, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  // inicializar lcd
  lcd.init();
  lcd.backlight();  
  lcd.setCursor(0, 0);
  lcd.print("Iniciando..."); 
  delay(2000);  
}

void loop() {
  int value = analogRead(sensorData); 
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ppm: ");
  lcd.setCursor(7, 0); 
  lcd.print(value);

  delay(1000); 
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

