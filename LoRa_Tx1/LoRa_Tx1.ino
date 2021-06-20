#include <LoRa.h>

int pot = A1;

int potVal;

int SyncWord = 0x22;

void setup()
{
  Serial.begin(9600);
  pinMode(pot, INPUT);
  while (!Serial)
    ;
  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  LoRa.setSpreadingFactor(12);     // ranges from 6-12,default 7 see API docs
  LoRa.setSignalBandwidth(62.5E3); // for -139dB (page - 112)
  LoRa.setCodingRate4(8);          // for -139dB (page - 112)
  LoRa.setSyncWord(SyncWord);
}

void loop()
{

  potVal = map(analogRead(pot), 0, 1024, 0, 255);
  Serial.println("Sending packet: ");

  // send packet
  if (LoRa.beginPacket() == 1)
  {
    // ready to transmit
    LoRa.print("P1:");
    LoRa.print(potVal);
    if (LoRa.endPacket() == 1)
    {
      //success
      Serial.print("P1:");
      Serial.print(potVal);
      Serial.println("");
      delay(random(1000, 2000));
      return;
    }
  }
  delay(random(1000, 2000));
}
