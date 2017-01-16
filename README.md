# Estación meteorológica con Wemos mini
Pruebas de creación de un repositorio en github para albergar los ficheros de la estación
Vamos a programar la placa Wemos desde el IDE Arduino que descargaremos de https://www.arduino.cc/en/Main/Software. 
Después seguimos el procedimiento que indican en https://fabricadigital.org/2015/11/programando-el-modulo-wifi-esp8266-con-arduino-ide/ para añadir las placas basadas en ESP8266, entre ellas la Wemos.

Comprobación del sensor de temperatura y humedad DHT22 (el blanco). 
Tenemos que descargar la librería para el DHT22 http://forum.arduino.cc/index.php?action=dlattach;topic=58531.0;attach=98867
Para instalarla, una vez descargado el fichero hay que descomprimirlo y copiar a la carpeta libraries dentro de arduino.

En esta librería vienen varios ejemplos. Abrimos el dht22_test.ino y adaptamos el pin donde tenemos conectado el sensor DHT22, que en el caso de la placa original Wemos es D4. En la línea donde pone #define DHT22_PIN 5 lo cambiamos por #define DHT22_PIN D4.
En el menú herramientas seleccionamos Placa "Wemos D1 R2 & mini" y seleccionamos el puerto en el que está conectado. Después, lo enviamos a la placa Wemos mini.
Una vez cargado el programa, abrimos el monitor serie (icono de lupa arriba a la derecha) y ajustamos la velocidad a la que venía en el bloque setup, en nuestro caso Serial.begin(115200);
Deberían aparecer la temperatura y la humedad por la consola serie.
Ya hemos comprobado que podemos leer el sensor DHT22.

Para comprobar la pantalla oled i2c tenemos que descargar la librería https://github.com/mcauser/Adafruit_SSD1306/archive/esp8266-64x48.zip que a su vez depende de https://github.com/adafruit/Adafruit-GFX-Library/archive/master.zip
Una vez que hemos descomprimido las dos y copiado a la carpeta libraries, podemos probar el ejemplo.
Abrimos Ejemplos > Adafruit SSD1306 > SSD 1306_64x48_i2c
Lo cargamos en el wemos y vemos las distintas posibilidades.

En caso de que no funcione tenemos que revisar el fichero Adafruit_SSD1306.h que está en la carpeta Adafruit_SSD1306-esp8266-64x48 dentro de libraries y comprobar que no están comentadas las siguientes líneas:
#define SSD1306_I2C_ADDRESS   0x3C
#define SSD1306_64_48
En mi caso la actualización automática de librerías me cambia esta librería por otra que no admite 64x48. En este caso, borro la carpeta SSD1306 y la vuelvo a cargar como al principio.

El fichero estacion_meteorologica_01.ino lee los datos del DHT22 y los muestra por la pantalla oled

El fichero wemos_aemet_ntp_01.ino se conecta a una red wifi fija, y a partir de esa conexión lee la hora de un servidor ntp.
También se conecta a aemet y lee datos meteorológicos de Zaragoza.
A continuación envía por el puerto serie a la consola de arduino dichos datos.
Usa la librería Time https://github.com/PaulStoffregen/Time comentada en http://playground.arduino.cc/code/time

Mejoras:
Usar WifiManager para seleccionar la red wifi
Comparar librerías para el DHT22, porque de vez en cuando aparecían lecturas erróneas -999 y en el foro del repositorio comentaban alguna solución
Comparar las medidas del DHT22 con otros sensores, entre ellos el BME280, y contrastar las medidas.
Limpiar y organizar los programas. Estructurarlos en pestañas por tareas.
Algunos trozos tienen muchos serial.print que me sirvieron para hacer debugging, pero quizás ahora se pueden quitar.
Optimizar la memoria. Los textos estáticos pueden liberar memoria ram. Se pueden elegir mejor los tipos de variables y los nombres
No hace falta leer los datos de aemet a piñón. Llamar a la subrrutina cada cierto tiempo, pero sin usar delay.
Cómo leer los datos actuales en vez de la previsión?
Tomar los datos de otra página?
Alternar pantallas para mostrar los datos.
