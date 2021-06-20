#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//int led = 3;
int val = 0;
int nss = 4;
int RESET = 15;
int dio0 = 2;
String full_string;
String inString = "";
int rssi;
int SyncWord = 0x22;
int inChar;

bool i = 0;
int priviousValue1 = 0,
    priviousValue2 = 0;
int liveValue1 = 0,
    liveValue2 = 0;

int Data1 = 0,
    Sensor_Data1 = 0;

void setup()
{
  LoRa.setPins(nss, RESET, dio0);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  //  pinMode(led,OUTPUT);
  while (!Serial)
    ;
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(433E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  LoRa.setSpreadingFactor(12);     // ranges from 6-12,default 7 see API docs
  LoRa.setSignalBandwidth(62.5E3); // for -139dB (page - 112)
  LoRa.setCodingRate4(8);          // for -139dB (page - 112)
  LoRa.setSyncWord(SyncWord);      // ranges from 0-0xFF, default 0x12, see API docs
  LoRa.setGain(6);
}

void loop()
{
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // read packet
    while (LoRa.available())
    {
      inChar = LoRa.read();
      inString += (char)inChar;
      val = inString.toInt();
    }

    Serial.print("Val: ");
    Serial.println(val);
    Serial.print("inChar: ");
    Serial.println(inChar);
    Serial.print("inString: ");
    Serial.println(inString);
    Serial.print("packetSize: ");
    Serial.println(packetSize);

    rssi = LoRa.packetRssi();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print(inString);
    display.setCursor(0, 8);
    display.print("RSSI: ");
    display.println(rssi);
    display.setCursor(0, 16);
    display.print(val);
    display.display();
    inString = "";
  }
}
