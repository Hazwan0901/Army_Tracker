#include <LoRa.h>

int pot = A1;

int val;

int SyncWord = 0x21;

void setup()
{
  Serial.begin(9600);
  pinMode(pot, INPUT);
  while (!Serial);
  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(12);           // ranges from 6-12,default 7 see API docs
  LoRa.setSignalBandwidth(62.5E3 );           // for -139dB (page - 112)
  LoRa.setCodingRate4(8);                   // for -139dB (page - 112)
  LoRa.setSyncWord(SyncWord);
}

void loop()
{

  //val = map(analogRead(pot), 0, 1024, 0, 255);
  Serial.println("Sending packet: ");
  val++;
  if(val>100){
    val = 0;
  }
  // send packet
  LoRa.beginPacket();
  LoRa.print("P2:");
  LoRa.print(val);
  LoRa.endPacket();

  Serial.print("Value:");
  Serial.print(val);
  Serial.println("");
  delay(50);
}
