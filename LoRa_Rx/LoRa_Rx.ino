//====================== INTIALIZE BYLNK ========================
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "6tilHJC8GhmvVRF2ERIZSA3jmiYL-WG1";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "pamattoya";
char pass[] = "satusampailima";

WidgetMap MapSoldier1(V2);

int Choose_Soldier;

BLYNK_WRITE(V3) {
  Choose_Soldier = param.asInt();
}
//======================== OLED VARIABLES ===============
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//======================== LORA VARIABLES ===============
#include <LoRa.h>

int val = 0;
int nss = 4;
int RESET = 15;
int dio0 = 2;

int SyncWord = 0x22;

int rssi;
int rxByte;

uint8_t rxBuffer[19];

uint8_t POLL_NODE = 0;
uint8_t TX_MODE = 1;
//======================= Delay Without Delay ===============
unsigned long previousMillis = 0,
              currentMillis = 0;
const long interval = 5000;

unsigned long previousMillis1 = 0,
              currentMillis1 = 0;
const long interval1 = 1000;

int delay_Count = 0;
int enable_Request = 0;
int request_Flag = 0;
//======================== Data From Transmitter ===============
uint8_t address;
float bodyTemp;
float heartRate;
uint8_t emergency;
float lat;
float lng;
uint8_t checksum;
//======================== Data From Transmitter 1 ===============
float bodyTemp_Node1;
float heartRate_Node1;
uint8_t emergency_Node1;
float lat_Node1;
float lng_Node1;
int Enable_Noty_Node1 = 0;
//======================== Data From Transmitter 2 ===============
float bodyTemp_Node2;
float heartRate_Node2;
uint8_t emergency_Node2;
float lat_Node2;
float lng_Node2;
int Enable_Noty_Node2 = 0;
//======================== VOID SETUP ===============
void setup()
{
  LoRa.setPins(nss, RESET, dio0);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  //  pinMode(led,OUTPUT);
  while (!Serial);
  // Serial.println("LoRa Receiver");
  if (!LoRa.begin(433E6))
  {
    // Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(12);     // ranges from 6-12,default 7 see API docs
  LoRa.setSignalBandwidth(62.5E3); // for -139dB (page - 112)
  LoRa.setCodingRate4(8);          // for -139dB (page - 112)
  LoRa.setSyncWord(SyncWord);      // ranges from 0-0xFF, default 0x12, see API docs

  Serial.print("SSID:");
  Serial.println(ssid);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(2, 1);
  display.print("Connecting to");
  display.setTextSize(1);
  display.setCursor(2, 15);
  display.print(ssid);
  display.display();

  Blynk.begin(auth, ssid, pass);

  Serial.print("IP:");
  Serial.println(WiFi.localIP().toString().c_str());

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(2, 1);
  display.print("Wifi Connected");
  display.setTextSize(1);
  display.setCursor(2, 15);
  display.print("with IP:");
  display.print(WiFi.localIP());
  display.display();
}
//======================== VOID LOOP ===============
void loop()
{
  Blynk.run();
  //Request Data from transmitter
  Request_Data();
  //Extract data after received data from transmitter
  Extract_Data();
  //Send Data to Blynk App
  Send_Data();
}

//======================== Request Data From Transmitter ===============
void Request_Data()
{
  //Request Data from Transmitter with interval
  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (TX_MODE == 1 )
    {
      
      if (LoRa.beginPacket() == 1)
      {
        uint8_t buff[1] = {1};
        LoRa.write(buff, 1);
        if (LoRa.endPacket() == 1)
        {
          Serial.print("Get data from node : 1");
        }
      }
      TX_MODE = 0;
    } else {
      if (LoRa.beginPacket() == 1)
      {
        uint8_t buff[1] = {2};
        LoRa.write(buff, 1);
        if (LoRa.endPacket() == 1)
        {
          Serial.print("Get data from node : 2");
        }
      }
      TX_MODE = 1;
    }
  }
  /*
    if (TX_MODE == 1 )
    {

    if (LoRa.beginPacket() == 1)
    {
      uint8_t buff[1] = {POLL_NODE + 1};
      LoRa.write(buff, 1);
      if (LoRa.endPacket() == 1)
      {
        Serial.print("Get data from node  :");
        Serial.println(POLL_NODE + 1);
        POLL_NODE = (POLL_NODE + 1) % 2;
      }
      TX_MODE = 0;
      return;
    }
    }

    timeout -= 1;
    if (timeout == 0)
    {
    timeout = 1000000;
    TX_MODE = 1;
    }
  */
}
//======================== Extract Data From Transmitter ===============
void Extract_Data() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    //TX_MODE = 1;
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
    /*
      if (rxCount == 19)
      {
      Serial.println("Complete packet received");
      }
      uint8_t chksum = (uint8_t)(verifyChecksum);
      /*
      if (chksum == 0xFF)
      {
      Serial.println("Packet OK!");
      }
      else
      {
      Serial.println("Packet Corrupted!");
      }
    */
    //Serial.write(rxBuffer, 19);
    Serial.println("...............");
    address = rxBuffer[0];

    ((byte *)&heartRate)[0] = rxBuffer[1];
    ((byte *)&heartRate)[1] = rxBuffer[2];
    ((byte *)&heartRate)[2] = rxBuffer[3];
    ((byte *)&heartRate)[3] = rxBuffer[4];

    ((byte *)&bodyTemp)[0] = rxBuffer[5];
    ((byte *)&bodyTemp)[1] = rxBuffer[6];
    ((byte *)&bodyTemp)[2] = rxBuffer[7];
    ((byte *)&bodyTemp)[3] = rxBuffer[8];

    emergency = rxBuffer[9];

    ((byte *)&lat)[0] = rxBuffer[10];
    ((byte *)&lat)[1] = rxBuffer[11];
    ((byte *)&lat)[2] = rxBuffer[12];
    ((byte *)&lat)[3] = rxBuffer[13];


    ((byte *)&lng)[0] = rxBuffer[14];
    ((byte *)&lng)[1] = rxBuffer[15];
    ((byte *)&lng)[2] = rxBuffer[16];
    ((byte *)&lng)[3] = rxBuffer[17];

    checksum = rxBuffer[18];

    Save_Data();


  }
}
//======================== Save Data From Transmitter ===============
void Save_Data() {
  /*
    Serial.println("==================== Data ================");
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
    Serial.println("==================== END ================");
  */
  switch (address)
  {
    case 1:
      heartRate_Node1 = heartRate;
      bodyTemp_Node1 = bodyTemp;
      emergency_Node1 = emergency;
      lat_Node1 = lat;
      lng_Node1 = lng;
      Display_data_Node1();
      break;

    case 2:
      heartRate_Node2 = heartRate;
      bodyTemp_Node2 = bodyTemp;
      emergency_Node2 = emergency;
      lat_Node2 = lat;
      lng_Node2 = lng;
      Display_data_Node2();
      break;
  }
  rssi = LoRa.packetRssi();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Received Data");
  display.setCursor(0, 8);
  display.print("From node ");
  display.print(address);
  display.setCursor(0, 16);
  display.print("RSSI: ");
  display.println(rssi);
  display.display();
}
//======================== Display Data Node 1 ===============
void Display_data_Node1() {
  Serial.println("==================== Data ================");
  Serial.println("Data Node 1");

  Serial.print("Heart Rate : ");
  Serial.println(heartRate_Node1);

  Serial.print("Body Temperature : ");
  Serial.println(bodyTemp_Node1);

  Serial.print("Emergency : ");
  Serial.println(emergency_Node1);

  Serial.print("Latitude : ");
  Serial.println(lat_Node1, 6);

  Serial.print("Longitude : ");
  Serial.println(lng_Node1, 6);
  Serial.println("==================== END ================");
}
//======================== Display Data Node 2 ===============
void Display_data_Node2() {
  Serial.println("==================== Data ================");
  Serial.println("Data Node 2");

  Serial.print("Heart Rate : ");
  Serial.println(heartRate_Node2);

  Serial.print("Body Temperature : ");
  Serial.println(bodyTemp_Node2);

  Serial.print("Emergency : ");
  Serial.println(emergency_Node2);

  Serial.print("Latitude : ");
  Serial.println(lat_Node2, 6);

  Serial.print("Longitude : ");
  Serial.println(lng_Node2, 6);
  Serial.println("==================== END ================");

}
//======================== Send data to blynk APP ===============
void Send_Data()
{
  currentMillis1 = millis();

  if (currentMillis1 - previousMillis1 >= interval1) {
    previousMillis1 = currentMillis1;

    Blynk.virtualWrite(V0, bodyTemp_Node1);
    Blynk.virtualWrite(V1, heartRate_Node1);

    Blynk.virtualWrite(V4, bodyTemp_Node2);
    Blynk.virtualWrite(V3, heartRate_Node2);

    MapSoldier1.location(1, lat_Node1, lng_Node1, "Soldier 1");
    MapSoldier1.location(2, lat_Node2, lng_Node2, "Soldier 2");
  }

  if (Enable_Noty_Node1 == 0 && emergency_Node1 == 1)
  {
    //Blynk.notify("Emergency! Soldier 1 need Help");
    Enable_Noty_Node1 = 1;
  } else {
    Enable_Noty_Node1 = 0;
  }

  if (Enable_Noty_Node2 == 0 && emergency_Node2 == 1)
  {
    Blynk.notify("Emergency! Soldier 2 need Help");
    Enable_Noty_Node2 = 1;
  } else {
    Enable_Noty_Node2 = 0;
  }
}
