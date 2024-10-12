#include <EEPROM.h>

struct settings
{
  char ssid[30];
  char password[30];
  char endpoint[100];
  char requestIntervalTime[10];
} user_wifi = {};

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(sizeof(struct settings));

  // Borrar los datos en la EEPROM escribiendo ceros
  memset(&user_wifi, 0, sizeof(user_wifi)); // Establece todos los campos en cero
  EEPROM.put(0, user_wifi);                 // Sobreescribe en la EEPROM
  EEPROM.commit();                          // Guarda los cambios

  Serial.println("Datos de la EEPROM borrados.");
}

void loop()
{
  // No hay necesidad de código en el loop para esta tarea
}
