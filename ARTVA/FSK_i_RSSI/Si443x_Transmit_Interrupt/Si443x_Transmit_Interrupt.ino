#include <RadioLib.h>
#include <SPI.h>

Si4432 radio = new Module(D8, D3, D4);

int transmissionState;
volatile bool transmittedFlag;
int state;

#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif

void setFlag(void) {
  transmittedFlag = true;
}

void initialize(void) {
  transmissionState = RADIOLIB_ERR_NONE;
  transmittedFlag = false;
}

void config(void) {
  Serial.print(F("[Si4432] Initializing ... "));

  state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } 
  else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

  if (radio.setFrequency(433) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    Serial.println(F("[Si4432] Selected frequency is invalid for this module!"));
    while (true) { delay(10); }
  }

  radio.setBitRate(1);
  radio.setOutputPower(20);
  radio.setPreambleLength(2000);
}

void startTransmit(void) {
  radio.setPacketSentAction(setFlag);
  Serial.print(F("[Si4432] Sending first packet ... "));
  byte byteArr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  transmissionState = radio.startTransmit(byteArr, 8);
}

void transmit(void) {
  if(transmittedFlag) {
    transmittedFlag = false;

    if (transmissionState == RADIOLIB_ERR_NONE) {
      Serial.println(F("transmission finished!"));

    } 
    else {
      Serial.print(F("failed, code "));
      Serial.println(transmissionState);
    }
    radio.finishTransmit();

    //delay(700);

    Serial.print(F("[Si4432] Sending another packet ... "));
    byte byteArr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    transmissionState = radio.startTransmit(byteArr, 8);
  }
}

void setup() {
  Serial.begin(9600);

  initialize();
  config();

  startTransmit();
}

void loop() {
  transmit();
}
