#include <LoRa.h>

int pot = A1;

int potVal;

int SyncWord = 0x22;

int dutyDuration = 10000;

//dummy data:
uint8_t address = 2;     // 1 byte
float heartRate = 90.01; // 4 bytes
float bodyTemp = 37.5;   // 4 bytes
uint8_t emergency = 0;   // 1 byte
float lat = 2.567478;    // 4 bytes
float lng = 104.602458;  // 4 bytes
uint8_t checksum = 0;    // 1 bytes

// total bytes = 19 bytes
uint8_t buffer[19];

void setup()
{
  Serial.begin(9600);
  pinMode(pot, INPUT);
  while (!Serial)
    ;
  // Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6))
  {
    // Serial.println("Starting LoRa failed!");
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
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // read packet
    while (LoRa.available())
    {
      rxByte = LoRa.read();
      if (rxByte < 0)
      {
        continue;
      }
      if (rxByte == address)
      {
        // Serial.println("Sending packet: ");
        // send packet
        buffer[0] = address;

        buffer[1] = ((byte *)&heartRate)[0];
        buffer[2] = ((byte *)&heartRate)[1];
        buffer[3] = ((byte *)&heartRate)[2];
        buffer[4] = ((byte *)&heartRate)[3];

        buffer[5] = ((byte *)&bodyTemp)[0];
        buffer[6] = ((byte *)&bodyTemp)[1];
        buffer[7] = ((byte *)&bodyTemp)[2];
        buffer[8] = ((byte *)&bodyTemp)[3];

        buffer[9] = emergency;

        buffer[10] = ((byte *)&lat)[0];
        buffer[11] = ((byte *)&lat)[1];
        buffer[12] = ((byte *)&lat)[2];
        buffer[13] = ((byte *)&lat)[3];

        buffer[14] = ((byte *)&lng)[0];
        buffer[15] = ((byte *)&lng)[1];
        buffer[16] = ((byte *)&lng)[2];
        buffer[17] = ((byte *)&lng)[3];

        checksum = 0;
        for (int i = 0; i < 18; i++)
        {
          checksum = (uint8_t)(checksum + buffer[i]);
        }
        checksum = 0xFF - checksum;
        buffer[18] = checksum;

        Serial.write(buffer, 19);
        if (LoRa.beginPacket() == 1)
        {
          // random backoff
          // int _delay = random(0, dutyDuration / 2);
          // Serial.print("Backoff for : ");
          // Serial.print(_delay);
          // Serial.println(" ms");
          // delay(_delay);
          LoRa.write(buffer, 19);
          if (LoRa.endPacket() == 1)
          {
            //success
            // Serial.print("P1:");
            // Serial.println(potVal);
          }
          // delay(dutyDuration - _delay);

          return;
        }
      }
    }
  }