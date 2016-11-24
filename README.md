# Estación meteorológica con Wemos mini
Pruebas de creación de un repositorio en github para albergar los ficheros de la estación

El fichero estacion_meteorologica_01.ino lee los datos del DHT22 y los muestra por la pantalla oled
Usa librerías 
  para el DHT22 http://forum.arduino.cc/index.php?action=dlattach;topic=58531.0;attach=98867
  y para la pantalla oled i2c https://github.com/mcauser/Adafruit_SSD1306/archive/esp8266-64x48.zip
  que a su vez depende de https://github.com/adafruit/Adafruit-GFX-Library/archive/master.zip

El fichero wemos_aemet_ntp_01.ino se conecta a una red wifi fija, y a partir de esa conexión lee la hora de un servidor ntp.
También se conecta a aemet y lee datos meteorológicos de Zaragoza.
A continuación envía por el puerto serie a la consola de arduino dichos datos.
Usa la librería Time https://github.com/PaulStoffregen/Time comentada en http://playground.arduino.cc/code/time
