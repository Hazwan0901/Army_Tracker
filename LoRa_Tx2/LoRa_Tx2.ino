#include <LoRa.h>

int SyncWord = 0x22;

int dutyDuration = 10000;

//dummy data:
uint8_t address = 2;      // 1 byte
double heartRate = 90.01; // 4 bytes
double bodyTemp = 37.5;   // 4 bytes
uint8_t emergency = 0;    // 1 byte
double lat = 1.567478;    // 4 bytes
double lng = 103.602458;  // 4 bytes
uint8_t checksum = 0;     // 1 bytes

// total bytes = 19 bytes
uint8_t buffer = [19];

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

  bufffer[0] = address;
  buffer[1] = (uint8_t)(heartRate >> 24);
  buffer[2] = (uint8_t)(heartRate >> 16);
  buffer[3] = (uint8_t)(heartRate >> 8);
  buffer[4] = (uint8_t)(heartRate);
  buffer[5] = (uint8_t)(bodyTemp >> 24);
  buffer[6] = (uint8_t)(bodyTemp >> 16);
  buffer[7] = (uint8_t)(bodyTemp >> 8);
  buffer[8] = (uint8_t)(bodyTemp);
  buffer[9] = emergency;
  buffer[10] = (uint8_t)(lat >> 24);
  buffer[11] = (uint8_t)(lat >> 16);
  buffer[12] = (uint8_t)(lat >> 8);
  buffer[13] = (uint8_t)(lat);
  buffer[14] = (uint8_t)(lng >> 24);
  buffer[15] = (uint8_t)(lng >> 16);
  buffer[16] = (uint8_t)(lng >> 8);
  buffer[17] = (uint8_t)(lng);

  checksum = 0;
  for (int i = 0; i < 18; i++)
  {
    checksum = (uint8_t)(checksum + bufffer[i]);
  }
  checksum = 0xFF - checksum;
  buffer[18] = checksum;

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
    LoRa.write(buffer, 19);
    if (LoRa.endPacket() == 1)
    {
      //success
      Serial.print("P1:");
      Serial.println(potVal);
    }
    delay(dutyDuration - _delay);

    return;
  }
}
