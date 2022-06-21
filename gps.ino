/*
  Project: ESP8266 ESP-12E module, NEO-6M GY-GPS6MV2 GPS module
  Function: GPS data web server. This sketch listen to the GPS serial port,
  and when data received from the module, it's displays GPS data on web page

  ESP8266 ESP-12E module -> NEO-6M GY-GPS6MV2 GPS module
  VV (5V)     VCC
  GND         GND
  D1 (GPIO5)  RX
  D2 (GPIO4)  TX


*/


#define BLYNK_TEMPLATE_ID "TMPLd26xKDoS"
#define BLYNK_DEVICE_NAME "Remote Patient Monitoring System"
#define BLYNK_AUTH_TOKEN "ec1-xwth3S-LfperNwrpOk0dUgHCfyNF"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

BlynkTimer timer;
TinyGPSPlus gps;
SoftwareSerial SerialGPS(12, 14);  // iaitu GPIO12=D6 > TX GPS & GPIO14=D5 > RX GPS,
//SoftwareSerial SerialGPS(12, 14);  // iaitu GPIO16=D0 > TX GPS & GPIO15=D8 > RX GPS,
//SoftwareSerial SerialGPS(4, 5);  // asal (4, 5), iaitu GPIO4=D2 > TX GPS & GPIO5=D1 > RX GPS,

const char* ssid = "afa2020_2.4Ghz@unifi";
const char* password = "vae585910";
char auth[] = BLYNK_AUTH_TOKEN;

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;
String date_str , time_str , lat_str , lng_str;


WiFiServer server(80);

void myTimerEvent()
{
  while (SerialGPS.available() > 0) //while data is available
    if (gps.encode(SerialGPS.read())) //read gps data
    {
      if (gps.location.isValid()) //check whether gps location is valid
      {
        Latitude = gps.location.lat();
        lat_str = String(Latitude , 6); // latitude location is stored in a string
        Longitude = gps.location.lng();
        lng_str = String(Longitude , 6); //longitude location is stored in a string
        Blynk.virtualWrite(V7, lat_str);
        Blynk.virtualWrite(V8, lng_str);
        Serial.println(lat_str);
        Serial.println(lng_str);
        Serial.print("Number os satellites in use = ");
        Serial.println(gps.satellites.value());

      }
      else {
        Blynk.virtualWrite(V7, 0);
        Blynk.virtualWrite(V8, 0);
        Serial.print("Number os satellites in use = ");
        Serial.println(gps.satellites.value());


      }

    }


}

void setup()
{
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println();
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
  timer.setInterval(1000L, myTimerEvent);

}

void loop()
{
  Blynk.run();

  timer.run();

  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 6);
      }

      if (gps.date.isValid())
      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
          DateString = '0';
        DateString += String(date);

        DateString += " / ";

        if (month < 10)
          DateString += '0';
        DateString += String(month);
        DateString += " / ";

        if (year < 10)
          DateString += '0';
        DateString += String(year);
      }

      if (gps.time.isValid())
      {
        TimeString = "";
        hour = gps.time.hour() + 8; //adjust UTC
        minute = gps.time.minute();
        second = gps.time.second();

        if (hour < 10)
          TimeString = '0';
        TimeString += String(hour);
        TimeString += " : ";

        if (minute < 10)
          TimeString += '0';
        TimeString += String(minute);
        TimeString += " : ";

        if (second < 10)
          TimeString += '0';
        TimeString += String(second);
      }

    }
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  //Response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>GY-NEO-6M GPS Readings</title> <style>";
  s += "table, th, td {border: 1px solid blue;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER>GY-NEO-6M GPS Readings</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += LatitudeString;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += LongitudeString;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += DateString;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += TimeString;
  s += "</td>  </tr> </table> ";


  if (gps.location.isValid())
  {
    s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += LatitudeString;
    s += "+";
    s += LongitudeString;
    s += """ target=""_top"">Click here</a> to open the location in Google Maps.</p>";
  }

  s += "</body> </html> \n";

  client.print(s);
  delay(100);

}
