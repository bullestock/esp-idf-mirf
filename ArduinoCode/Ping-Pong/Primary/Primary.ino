//primary program

#include "Mirf.h"
#include "printf.h"

Nrf24l Mirf = Nrf24l(10, 9); // CE,CSN

union MYDATA_t {
  byte value[32];
  char now_time[32];
};

MYDATA_t mydata;

void setup()
{
  Serial.begin(115200);
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.payload = sizeof(mydata.value); // Set the payload size
  Mirf.channel = 90;                   //Set the channel used
  Mirf.config();

  // Set my own address to RX_ADDR_P1
  Mirf.setRADDR((byte *)"ABCDE");

  // Set destination address to TX_ADDR
  // Set ACK waiting address to RX_ADDR_P0
  Mirf.setTADDR((byte *)"FGHIJ");

  // Print current settings
  printf_begin();
  Mirf.printDetails();

  // Clear RX FiFo
  while(1) {
    if (Mirf.dataReady() == false) break;
    Mirf.getData(mydata.value);
  }
  
}

void loop()
{
  sprintf(mydata.now_time,"now is %lu",micros());
  unsigned long startMillis = millis();
  Mirf.send(mydata.value);
  Serial.print("Wait for sending.....");
  // Verify send was successful
  if (Mirf.isSend()) {
    Serial.print("Send success:");
    Serial.println(mydata.now_time);

    // Wait for received data
    Serial.print("Wait for response.....");
    while(1) {
      if (Mirf.dataReady()) break;
    }
    Mirf.getData(mydata.value);
    unsigned long diffMillis = millis() - startMillis;
    Serial.print("Got response:");
    Serial.print(mydata.now_time);
    Serial.print(" Elapsed:");
    Serial.print(diffMillis);
    Serial.println(" mSec");

  } else {
    Serial.println("Send fail:");
  }
  delay(1000);
}
