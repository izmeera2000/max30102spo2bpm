

#include <Adafruit_GFX.h>        //OLED libraries
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ESP8266WiFi.h>  // wifi
#include <SoftwareSerial.h>
#include "ThingSpeak.h"
#include <TinyGPS++.h>

#define REPORTING_PERIOD_MS     1000


PulseOximeter pox;

uint32_t tsLastReport = 0;


TinyGPSPlus gps;
SoftwareSerial SerialGPS(2, 0);  //rx gps = d4 , tx gps =d3
WiFiClient client;
const char* ssid = "OPPORTUNITY";
const char* pass = "12345679";
//----------- Channel details ----------------//
unsigned long Channel_ID = 1704864; // Your Channel ID
const char * myWriteAPIKey = "3VJIL0OO5404LO1Z"; //Your write API key
//-------------------------------------------//
float Latitude , Longitude;
String  LatitudeString , LongitudeString;

//OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels 32
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)

static const unsigned char PROGMEM logo2_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E, 0x02, 0x10, 0x0C, 0x03, 0x10,              //Logo2 and Logo3 are two bmp pictures that display on the OLED if called
  0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40, 0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
  0x02, 0x08, 0xB8, 0x04, 0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
  0x00, 0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,
};

static const unsigned char PROGMEM logo3_bmp[] =
{ 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
  0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
  0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
  0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
  0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
  0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
  0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
  0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00
};

//LED
#define LED_pin7 D7           // tentukan nama device pada pin
#define LED_pin6 D6
#define LED_pin5 D5



// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  Serial.println("Beat!");
}

void setup()
{
  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);

  SerialGPS.begin(9600);

  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Wifi Connected ");
  Serial.print(WiFi.localIP());



  //OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
  display.display();
  delay(3000);
  //  Serial.begin(115200);

  //asalnya
  //  Serial.begin(115200);

  Serial.print("Initializing pulse oximeter..");

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }

  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);

  //LED
  pinMode(LED_pin7, OUTPUT);  // declara pin samada input @ output
  pinMode(LED_pin6, OUTPUT);
  pinMode(LED_pin5, OUTPUT);

}

void loop()
{


  // Make sure to call update as fast as possible
  pox.update();
  float BPM = pox.getHeartRate();
  uint8_t  SPO2 = pox.getSpO2();
  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Heart rate:");
    Serial.print(BPM);
    Serial.print("bpm    SpO2:");  //"bpm / SpO2:"
    Serial.print(SPO2);
    Serial.println("%");
    //    ThingSpeak.writeField(myChannelNumber, 1, pox.getHeartRate(), myWriteAPIKey);
    //    ThingSpeak.writeField(myChannelNumber, 2, pox.getSpO2(), myWriteAPIKey);
    tsLastReport = millis();

    while (SerialGPS.available() > 0) {
      if (gps.encode(SerialGPS.read())) {
        Serial.println("Searching");


        if (gps.location.isValid())
        {
          Latitude = gps.location.lat();
          LatitudeString = String(Latitude , 8);
          Longitude = gps.location.lng();
          LongitudeString = String(Longitude , 8);

          Serial.println(LatitudeString);
          Serial.println(LongitudeString);


          ThingSpeak.writeField(Channel_ID, 1, BPM, myWriteAPIKey);
          ThingSpeak.writeField(Channel_ID, 2, SPO2, myWriteAPIKey);

          ThingSpeak.writeField(Channel_ID, 3, LatitudeString, myWriteAPIKey);
          ThingSpeak.writeField(Channel_ID, 4, LongitudeString, myWriteAPIKey);
          Serial.println("Sent");
        }

      }
    }

    //    long irValue = pox.getIR();    //Reading the IR value it will permit us to know if there's a finger on the sensor or not
    //Also detecting a heartbeat
    //if(irValue > 7000){                                           //If a finger is detected
    display.clearDisplay();                                   //Clear the display
    display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);       //Draw the first bmp picture (little heart)
    display.setTextSize(1);                                   //Near it display the average BPM you can display the BPM if you want
    display.setTextColor(WHITE);
    display.setCursor(50, 0);
    display.println("BPM");
    display.setCursor(50, 18);
    display.println(BPM);
    display.setCursor(90, 0);    //80,0
    display.println("SpO2");
    display.setCursor(90, 18);   // 82,18
    display.println(SPO2);

    display.display();

    tone(15, 1000);                                       // set(0,1000) utk nodmcu. And tone the buzzer for a 100ms you can reduce it it will be better
    delay(100);
    noTone(15);


    if (BPM < 60)
    {
      digitalWrite(LED_pin5, HIGH);  //LED MERAH on
      digitalWrite(LED_pin7, LOW);  //LED HIJAU on
      digitalWrite(LED_pin6, LOW);  //LED KUNING on
    }
    if (BPM > 100)
    {
      digitalWrite(LED_pin6, HIGH);  //LED KUNING on
      digitalWrite(LED_pin5, LOW);  //LED MERAH on
      digitalWrite(LED_pin7, LOW);  //LED HIJAU on
    }
    if (BPM > 60 && BPM < 100)
    {
      digitalWrite(LED_pin7, HIGH);  //LED HIJAU on
      digitalWrite(LED_pin5, LOW);  //LED MERAH on
      digitalWrite(LED_pin6, LOW);  //LED KUNING on
    }
  }
}
