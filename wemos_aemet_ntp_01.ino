/*
   TimeNTP_ESP8266WiFi.ino
   Example showing time sync to NTP time source

   This sketch uses the ESP8266WiFi library
*/

#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char ssid[] = "********";  //  your network SSID (name)
const char pass[] = "********";       // your network password

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

const int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)


WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);

// Use WiFiClient class to create TCP connections
WiFiClient client;

char server[] = "www.aemet.es";

String lectura = "";
String estado_cielo = "";
char ciudad[9];
byte elaborado_hora;
byte elaborado_min;
byte prob_prec;
char estado[17];
char viento[3];
byte viento_vel;
byte temp_max;
byte temp_min;

char c;
boolean datosparseados = false;

void setup()
{
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  delay(250);
  Serial.println("TimeNTP Example");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
}

time_t prevDisplay = 0; // when the digital clock was displayed

void loop()
{
  leeaemet();
  if (datosparseados == true) {
    muestraactual();
  }
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();
    }
  }
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}


// this method makes a HTTP connection to the server:
void leeaemet() {
  Serial.println("Entra en leeaemet");
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    client.println("GET /xml/municipios/localidad_50297.xml HTTP/1.1");
    client.println("Host: www.aemet.es");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    delay(1000);
    parseaxml();
  }
  else {
    // if you couldn't make a connection:
    client.stop();
    Serial.println("FALLA CONEXION CON aemet.es");  // if there's no net connection, but there was one last time
    delay(2000);
  }
  Serial.println("Sale de leeaemet");
}

void parseaxml()
{
  Serial.println("Entra en parseaxml");
  if (client.available()) {
    lee_info();
    if (has(ciudad, "Zaragoza")) {
      Serial.println("ACTUALIZA DATOS DESDE aemet.es");  // if there's no net connection, but there was one last time
      delay(1000);
      lee_actuales();
      datosparseados = true;
    }
  }
  else  {
    Serial.println("NO PUEDE ACCEDER A DATOS INTERNET");  // if there's no net connection, but there was one last time
    datosparseados = false;
  }
  client.flush();
  client.stop();
  Serial.println("Sale de parseaxml");
}

void lee_info()
{
  Serial.println("Entra en lee_info");
  // Extrae la cabecera, ciudad y hora de elaboracin del informe
  client.find("<elaborado>");
  client.find("T");
  elaborado_hora = client.parseInt();
  Serial.print(elaborado_hora);
  Serial.print(":");
  client.find(":");
  elaborado_min = client.parseInt();
  Serial.println(elaborado_min);
  client.find("<nombre>");
  lectura = client.readStringUntil('<');
  Serial.println(lectura);
  lectura.toCharArray(ciudad, 9);
  Serial.println(ciudad);
  Serial.println("Sale de lee_info");
}

void lee_actuales()
{
  Serial.println("Entra en lee_actuales");
  if ((hour() >= 0) && (hour() < 6)) leepronostico_00_06();
  if ((hour() >= 6) && (hour() < 12)) leepronostico_06_12();
  if ((hour() >= 12) && (hour() < 18)) leepronostico_12_18();
  if ((hour() >= 18) && (hour() < 24)) leepronostico_18_24();
  Serial.println("Sale de lee_actuales");
}

void leepronostico_00_06()
{
  client.find("prob_precipitacion");
  client.find("00-06");
  c = client.read();
  prob_prec = client.parseInt();
  client.find("estado_cielo");
  client.find("00-06");
  client.find("descripcion=");
  c = client.read();
  estado_cielo = client.readStringUntil('"');
  client.find("<viento periodo");
  client.find("00-06");
  client.find("<direccion>");
  lectura = client.readStringUntil('<');
  lectura.toCharArray(viento, 3);
  client.find("<velocidad>");
  viento_vel = client.parseInt();
  client.find("<temperatura>");
  client.find("<maxima>");
  temp_max = client.parseInt();
  client.find("<minima>");
  temp_min = client.parseInt();
}

void leepronostico_06_12()
{
  client.find("prob_precipitacion");
  client.find("06-12");
  c = client.read();
  prob_prec = client.parseInt();
  client.find("estado_cielo");
  client.find("06-12");
  client.find("descripcion=");
  c = client.read();
  estado_cielo = client.readStringUntil('"');
  client.find("<viento periodo");
  client.find("06-12");
  client.find("<direccion>");
  lectura = client.readStringUntil('<');
  lectura.toCharArray(viento, 3);
  client.find("<velocidad>");
  viento_vel = client.parseInt();
  client.find("<temperatura>");
  client.find("<maxima>");
  temp_max = client.parseInt();
  client.find("<minima>");
  temp_min = client.parseInt();
}

void leepronostico_12_18()
{
  client.find("prob_precipitacion");
  client.find("12-18");
  c = client.read();
  prob_prec = client.parseInt();
  client.find("estado_cielo");
  client.find("12-18");
  client.find("descripcion=");
  c = client.read();
  estado_cielo = client.readStringUntil('"');
  client.find("<viento periodo");
  client.find("12-18");
  client.find("<direccion>");
  lectura = client.readStringUntil('<');
  lectura.toCharArray(viento, 3);
  client.find("<velocidad>");
  viento_vel = client.parseInt();
  client.find("<temperatura>");
  client.find("<maxima>");
  temp_max = client.parseInt();
  client.find("<minima>");
  temp_min = client.parseInt();
}

void leepronostico_18_24()
{
  client.find("prob_precipitacion");
  client.find("18-24");
  c = client.read();
  prob_prec = client.parseInt();
  client.find("estado_cielo");
  client.find("18-24");
  client.find("descripcion=");
  c = client.read();
  estado_cielo = client.readStringUntil('"');
  client.find("<viento periodo");
  client.find("18-24");
  client.find("<direccion>");
  lectura = client.readStringUntil('<');
  lectura.toCharArray(viento, 3);
  client.find("<velocidad>");
  viento_vel = client.parseInt();
  client.find("<temperatura>");
  client.find("<maxima>");
  temp_max = client.parseInt();
  client.find("<minima>");
  temp_min = client.parseInt();
}

void muestraactual()
{
  Serial.println("Entra en muestraactual");
  Serial.print("PRONOSTICO ");
  Serial.print(ciudad);
  Serial.print("a las ");
  if (elaborado_hora < 10) Serial.print(" 0 ");
  Serial.print(elaborado_hora);
  Serial.print(" : ");
  if (elaborado_min < 10) Serial.println("0");
  Serial.println(elaborado_min);
  Serial.print("prob_prec_18-24: ");
  Serial.print(prob_prec);
  Serial.println("%");
  Serial.println(estado_cielo);
  Serial.print("Viento:");
  Serial.print(viento_vel);
  Serial.print("km/h del ");
  Serial.println(viento);
  Serial.print("Maxima:");
  Serial.print(temp_max);
  Serial.print(" minima ");
  Serial.println(temp_min);
  Serial.println("Sale de muestraactual");
  delay(1000);
}

//   http://alexanderbrevig.com  Search haystack for the needle
bool has(char *haystack, char *needle)
{
  int compareOffset = 0;
  //iterate through the zero terminated string heystack
  while (*haystack) {
    if (*haystack == *needle) { //we might have found a match
      compareOffset = 0; //start at the current location in haystack
      //see if the string from that location matches the needle
      while (haystack[compareOffset] == needle[compareOffset]) {
        compareOffset++;
        if (needle[compareOffset] == '\0') {
          return true; //we have reached the end of needle and everything matched
        }
      }
    }
    haystack++; //increment the location in the string (the pointer)
  }
  return false; //no match was found
}


