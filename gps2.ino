#define BLYNK_TEMPLATE_ID "TMPLd26xKDoS"
#define BLYNK_DEVICE_NAME "Remote Patient Monitoring System"
#define BLYNK_AUTH_TOKEN "ec1-xwth3S-LfperNwrpOk0dUgHCfyNF"
#include <TinyGPS++.h> // library for GPS module
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(4, 5); // The serial connection to the GPS device
const char* ssid = "OPPORTUNITY"; //ssid of your wifi
const char* password = "12345679"; //password of your wifi
float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str;
#include <BlynkSimpleEsp8266.h>
BlynkTimer timer;

char auth[] = BLYNK_AUTH_TOKEN;
void myTimerEvent()
{
  while (ss.available() > 0) //while data is available
    if (gps.encode(ss.read())) //read gps data
    {
      if (gps.location.isValid()) //check whether gps location is valid
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6); // latitude location is stored in a string
        longitude = gps.location.lng();
        lng_str = String(longitude , 6); //longitude location is stored in a string
      }
      Serial.println(lat_str);
      Serial.println(lng_str);


    }
  Blynk.virtualWrite(V7, lat_str);
  Blynk.virtualWrite(V8, lng_str);

}
void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  Blynk.begin(auth, ssid, password);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //connecting to wifi
  while (WiFi.status() != WL_CONNECTED)// while wifi not connected
  {
    delay(500);
    Serial.print("."); //print "...."
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("Server started");
  Serial.println(WiFi.localIP());  // Print the IP address
  timer.setInterval(1000L, myTimerEvent);

}


void loop()
{
  Blynk.run();
  timer.run();



}
