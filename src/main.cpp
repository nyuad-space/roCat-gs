#include <config.h>

RH_RF95 rf95(RF95_CS, RF95_INT);

void setup() {

  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(RF95_RST, OUTPUT);
  digitalWrite(RF95_RST, LOW);

  while (!Serial)
  {
    delay(1);
  }

  if(Serial)
  {
    Serial.println("Serial Monitor Ready!");
  }
  else
  {
    Serial.println("Serial Monitor Not Ready!");
  }

  digitalWrite(RF95_RST, HIGH);
  delay(10);
  digitalWrite(RF95_RST, LOW);
  delay(10);

  if (!rf95.init()) {
    Serial.println("RFM95 Radio init() Failed.");
    while (1);
  }
  else
  {
    Serial.println("RFM95 Radio init() OK!");
  }

  if (!rf95.setFrequency(RF95_FREQ)) 
  {
    Serial.println("setFrequency failed.");
  }
  else
  {
    Serial.println("setFrequency success.");
  }

  //Range (5~25dBm) for RFM95
  rf95.setTxPower(23, true);

  Serial.print("RFM95 radio @");  Serial.print((int)RF95_FREQ);  Serial.println(" MHz");

}

void loop() {
    if (rf95.available())
    {
      uint8_t buf[120];
      uint8_t len = sizeof(buf);

      if (rf95.recv(buf, &len))
      {
          if (!len)
          {
            return;
          }
        //Print out the package recieved
        Serial.println((char *)buf);
        buf[len] = 0;

        // Serial.print(":3\n");
        // Serial.print("10000000000");
        // Serial.print(",");
        // Serial.println("10000000000");

        // Serial.print("Signal Strength: ");
        // Serial.println(rf95.lastRssi(), DEC);
        // Serial.println("");
      }
    }

    char data[RH_RF95_MAX_MESSAGE_LEN];
    static char endMarker = '\n';
    char receivedChar;
    int ndx = 0;
    memset(data, 32, sizeof(data));

    bool dataReceived = false;

    while (Serial.available() > 0)
    {
      receivedChar = Serial.read();
      if (receivedChar == endMarker)
      {
        data[ndx] = '\0';
        continue;
      }

      data[ndx] = receivedChar;
      ndx++;
      if (ndx >= RH_RF95_MAX_MESSAGE_LEN)
      {
        break;
      }
      dataReceived = true;
    }
    //Only send when second package arrives (If using the same way as RFM69) or change package to a single packet
    if (dataReceived)
    {
        rf95.send((uint8_t *)data, sizeof(data));
        rf95.waitPacketSent();
    }

    memset(data, 32, sizeof(data));
}