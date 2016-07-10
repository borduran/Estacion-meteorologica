/*********************************************************************
  Factoria Maker  
  estacion_meteorologica_01
  Estación meteorológica con DHT22 y pantalla i2c 

  Necesita la librería para el DHT22
  http://forum.arduino.cc/index.php?action=dlattach;topic=58531.0;attach=98867

  y para la pantalla oled i2c
  https://github.com/mcauser/Adafruit_SSD1306/archive/esp8266-64x48.zip
  que a su vez depende de
  https://github.com/adafruit/Adafruit-GFX-Library/archive/master.zip

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <dht.h>


// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 48)
#error("Altura incorrecta. Revisa Adafruit_SSD1306.h!");
#endif

// Define un objeto para guardar los datos del DHT22
dht DHT;

#define DHT22_PIN 2   // D4 GPIO2 Pin que usa la plaquita DHT22 de Wemos mini
struct
{
  uint32_t total;
  uint32_t ok;
  uint32_t crc_error;
  uint32_t time_out;
  uint32_t connect;
  uint32_t ack_l;
  uint32_t ack_h;
  uint32_t unknown;
} stat = { 0, 0, 0, 0, 0, 0, 0, 0};

void setup()   {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // inicializa la pantalla I2C con la dirección 0x3C
  display.display();
  display.clearDisplay();  // Borra la pantalla

  // Al iniciar, muestra el mensaje
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("DHT22");

  display.setCursor(0, 20);
  display.setTextSize(1);
  display.println("Factoria");
  display.println("Maker");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
}


void loop() {
  int chk = DHT.read22(DHT22_PIN);

  // Muestra los datos del DHT22 por la pantalla
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(DHT.humidity, 1);
  display.println("%");
  display.print(DHT.temperature, 1);
  display.write(247); // Muestra el símbolo de grado para la temperatura
  display.display();
  delay(2000);
}


