#include <LoRa.h>

int pot = A1;
int dutyDuration = 10000;

int counter = 1;

int SyncWord = 0x21;

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
  Serial.println("Sending packet: ");
  // send packet
  if (LoRa.beginPacket() == 1)
  {
    // random backoff
    int _delay = random(dutyDuration / 2, dutyDuration);
    Serial.print("Backoff for : ");
    Serial.print(_delay);
    Serial.println(" ms");
    delay(_delay);

    LoRa.print("P2:");
    LoRa.print(counter);
    if (LoRa.endPacket() == 1)
    {
      // success
      Serial.print("P2:");
      Serial.print(counter);
      Serial.println("");

      counter++;
      if (counter > 100)
      {
        counter = 1;
      }
    }

    delay(dutyDuration - _delay);
    return;
  }
}
