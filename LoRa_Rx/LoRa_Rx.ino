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
int rxByte;

bool i = 0;
int priviousValue1 = 0,
    priviousValue2 = 0;
int liveValue1 = 0,
    liveValue2 = 0;

int Data1 = 0,
    Sensor_Data1 = 0;

uint8_t rxBuffer[19];

void setup()
{
  LoRa.setPins(nss, RESET, dio0);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  //  pinMode(led,OUTPUT);
  while (!Serial)
    ;
  // Serial.println("LoRa Receiver");
  if (!LoRa.begin(433E6))
  {
    // Serial.println("Starting LoRa failed!");
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
    int rxCount = 0;
    long verifyChecksum = 0;

    while (LoRa.available())
    {
      rxByte = LoRa.read();
      if (rxByte < -1)
      {
        return;
      }
      verifyChecksum += rxByte;
      rxBuffer[rxCount++] = rxByte;
    }
    if (rxCount == 19)
    {
      // Serial.println("Complete packet received");
    }
    uint8_t chksum = (uint8_t)(verifyChecksum);
    if (chksum == 0xFF)
    {
      // Serial.println("Packet OK!");
    }
    else
    {
      // Serial.println("Packet Corrupted!");
    }
    Serial.write(rxBuffer, 19);

    uint8_t address = rxBuffer[0];

    float heartRate;
    ((byte *)&heartRate)[0] = rxBuffer[1];
    ((byte *)&heartRate)[1] = rxBuffer[2];
    ((byte *)&heartRate)[2] = rxBuffer[3];
    ((byte *)&heartRate)[3] = rxBuffer[4];

    float bodyTemp;
    ((byte *)&bodyTemp)[0] = rxBuffer[5];
    ((byte *)&bodyTemp)[1] = rxBuffer[6];
    ((byte *)&bodyTemp)[2] = rxBuffer[7];
    ((byte *)&bodyTemp)[3] = rxBuffer[8];

    uint8_t emergency = rxBuffer[9];

    float lat;
    ((byte *)&lat)[0] = rxBuffer[10];
    ((byte *)&lat)[1] = rxBuffer[11];
    ((byte *)&lat)[2] = rxBuffer[12];
    ((byte *)&lat)[3] = rxBuffer[13];

    float lng;
    ((byte *)&lng)[0] = rxBuffer[14];
    ((byte *)&lng)[1] = rxBuffer[15];
    ((byte *)&lng)[2] = rxBuffer[16];
    ((byte *)&lng)[3] = rxBuffer[17];

    uint8_t checksum = rxBuffer[18];

    // Serial.print("From node : ");
    // Serial.println(address);

    // Serial.print("Heart Rate : ");
    // Serial.println(heartRate);

    // Serial.print("Body Temperature : ");
    // Serial.println(bodyTemp);

    // Serial.print("Emergency : ");
    // Serial.println(emergency);

    // Serial.print("Latitude : ");
    // Serial.println(lat);

    // Serial.print("Longitude : ");
    // Serial.println(lng);

    // Serial.print("Checksum : ");
    // Serial.println(checksum);

    //    Serial.print("Val: ");
    //    Serial.println(val);
    //    Serial.print("inChar: ");
    //    Serial.println(inChar);
    //    Serial.print("inString: ");
    //    Serial.println(inString);
    //    Serial.print("packetSize: ");
    //    Serial.println(packetSize);

    rssi = LoRa.packetRssi();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    //    display.setCursor(0, 0);
    //    display.print(inString);
    display.setCursor(0, 8);
    display.print("RSSI: ");
    display.println(rssi);
    //    display.setCursor(0, 16);
    //    display.print(val);
    display.display();
    //    inString = "";
  }
}
