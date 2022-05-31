#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

TinyGPSPlus gps;
SoftwareSerial SerialGPS(2, 0);  //rx gps = d4 , tx gps =d3

const char* ssid = "OPPORTUNITY";
const char* password = "12345679";
//----------- Channel details ----------------//
unsigned long Channel_ID = 1704864; // Your Channel ID
const char * myWriteAPIKey = "3VJIL0OO5404LO1Z"; //Your write API key
//-------------------------------------------//
float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;

WiFiClient  client;

//WiFiServer server(80);
void setup()
{
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  Serial.begin(115200);
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

  //  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
}

void loop()
{


  while (SerialGPS.available() > 0) {
    Serial.println("Searching");

    if (gps.encode(SerialGPS.read())) {


      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 8);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 8);

        Serial.println(LatitudeString);
        Serial.println(LongitudeString);

        ThingSpeak.writeField(Channel_ID, 3, LatitudeString, myWriteAPIKey);
        ThingSpeak.writeField(Channel_ID, 4, LongitudeString, myWriteAPIKey);
        Serial.println("Sent");
      }

    }

  }
  //  WiFiClient client = server.available();

  //
  //  //Response
  //  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>NEO-6M GPS Readings</title> <style>";
  //  s += "table, th, td {border: 1px solid blue;} </style> </head> <body> <h1  style=";
  //  s += "font-size:300%;";
  //  s += " ALIGN=CENTER>NEO-6M GPS Readings</h1>";
  //  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  //  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  //  s += "width:50%";
  //  s += "> <tr> <th>Latitude</th>";
  //  s += "<td ALIGN=CENTER >";
  //  s += LatitudeString;
  //  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  //  s += LongitudeString;
  //  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  //  s += DateString;
  //  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  //  s += TimeString;
  //  s += "</td>  </tr> </table> ";
  //

  //  if (gps.location.isValid())
  //  {
  //    s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
  //    s += LatitudeString;
  //    s += "+";
  //    s += LongitudeString;
  //    s += """ target=""_top"">Click here</a> to open the location in Google Maps.</p>";
  //  }
  //
  //  s += "</body> </html> \n";

  //  client.print(s);
  delay(100);

}
