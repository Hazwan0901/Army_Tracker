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

int SyncWord = 0x22;

int rssi;
int rxByte;

uint8_t rxBuffer[19];

uint8_t POLL_NODE[] = {1};
uint8_t TX_MODE = 1;

long timeout = 100000;

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
  // LoRa.setGain(6);
}

void loop()
{

  if (TX_MODE == 1)
  {
    delay(2000);
    if (LoRa.beginPacket() == 1)
    {

      LoRa.write(POLL_NODE, 1);
      if (LoRa.endPacket() == 1)
      {
        Serial.print("Polling for node 1");
      }
      if (POLL_NODE[0] == 1)
      {
        POLL_NODE[0] == 2;
      }
      else
      {
        POLL_NODE[0] == 1;
      }
      TX_MODE = 0;
      return;
    }
  }

  timeout -= 1;
  if (timeout == 0)
  {
    timeout = 100000;
    TX_MODE = 1;
  }

  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    TX_MODE = 1;
    // read packet
    int rxCount = 0;
    long verifyChecksum = 0;

    while (LoRa.available())
    {
      rxByte = LoRa.read();
      if (rxByte < 0)
      {
        continue;
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
    //Serial.write(rxBuffer, 19);
    Serial.println("...............");
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

    Serial.print("From node : ");
    Serial.println(address);

    Serial.print("Heart Rate : ");
    Serial.println(heartRate);

    Serial.print("Body Temperature : ");
    Serial.println(bodyTemp);

    Serial.print("Emergency : ");
    Serial.println(emergency);

    Serial.print("Latitude : ");
    Serial.println(lat, 6);

    Serial.print("Longitude : ");
    Serial.println(lng, 6);

    Serial.print("Checksum : ");
    Serial.println(checksum);
    /*
    Serial.print("Val: ");
    Serial.println(val);
    Serial.print("inChar: ");
    Serial.println(inChar);
    Serial.print("inString: ");
    Serial.println(inString);
    Serial.print("packetSize: ");
    Serial.println(packetSize);
    */
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
