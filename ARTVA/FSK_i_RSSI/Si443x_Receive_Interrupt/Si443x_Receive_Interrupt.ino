#include <RadioLib.h>
#include <SPI.h>
#include <Wire.h>
#include <LSM6.h>
#include <U8g2lib.h>

Si4432 radio = new Module(D8, D3, D4);
LSM6 imu;
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

volatile bool receivedFlag;
int state;

float azimuth;
float elevation;
float lastAzimuthMaxRSSI;
float azimuthChange;

uint8_t rawRSSI;
uint8_t maxRSSI;

unsigned long lastPacketTime;

String str;

// ISR - flaga odebranego pakietu
#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif

void setFlag(void) {
    receivedFlag = true;
}

void initialize(void) {
  receivedFlag = false;
  lastPacketTime = 0.0;

  azimuth = 0.0; 
  elevation = 0.0; 
  lastAzimuthMaxRSSI = 0.0;  
  azimuthChange = 0.0;  

  maxRSSI = 0;
}

void config(void) {
    // Si4432 ----------------------------------------------------------
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
        Serial.println(F("[Si4432] Selected frequency is invalid!"));
        while (true) { delay(10); }
    }

    radio.setBitRate(1); 
    radio.setPreambleLength(2000);

    setRSSIThreshold(30); // -115 dbm
    setMaxRxGain();

    // LSM6DSO ---------------------------------------------------------
    Serial.println(F("[LSM6DSO] Initializing ... "));
    if (!imu.init()) {
      Serial.println("Failed to detect and initialize!");
      while (1);
    }
    imu.enableDefault();

    // OLED ------------------------------------------------------------
    u8g2.begin();

    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_ncenB14_tr);
    uint8_t textWidth = u8g2.getStrWidth("ARTVA"); 
    uint8_t xPos = (128 - textWidth) / 2; 
    u8g2.setCursor(xPos, 30); 
    u8g2.print("ARTVA"); 
    u8g2.sendBuffer(); 
}

void setRSSIThreshold(uint8_t thresholdRaw) {
    digitalWrite(D8, LOW);
    SPI.transfer(0x27 | 0x80);  // zapis do 0x27, czyli 0xA7 - 7 bit na 1 bo zapis
    SPI.transfer(thresholdRaw);
    digitalWrite(D8, HIGH);
}

bool isAboveThreshold() {
    digitalWrite(D8, LOW);
    SPI.transfer(0x04);  // status register
    uint8_t status = SPI.transfer(0x00);
    digitalWrite(D8, HIGH);
    return status & 0b00000010;  // Bit 1 = irssi
}

uint8_t readRSSI() {
    digitalWrite(D8, LOW);      
    SPI.transfer(0x26);        
    uint8_t rssiRaw = SPI.transfer(0x00); 
    digitalWrite(D8, HIGH);   
    return rssiRaw;
}

void setMaxRxGain() {
    digitalWrite(D8, LOW);
    SPI.transfer(0x69 | 0x80);  // zapis do 0x69
    SPI.transfer(0b01111111);  // Bit 6 = 1 (wyłącz AGC), 0b111111 = maksymalny gain
    digitalWrite(D8, HIGH);
}

float normalizeAngle(float angle) {
  while (angle < 0.0) {
    angle += 360.0;
  }
  while (angle >= 360.0) {
    angle -= 360.0;
  }
  return angle;
}

float normalizeAzimuth(float azimuth) {
  while (azimuth <= -180.0) {
    azimuth += 360.0;
  }
  while (azimuth > 180.0) {
    azimuth -= 360.0;
  }
  return azimuth;
}

void countAngle(void) {
  imu.read();

  // Oblicz azymut na podstawie przyspieszenia (akcelerometr)
  float accelX = imu.a.x;
  float accelY = imu.a.y;
  azimuth = atan2(accelY, accelX) * 180.0 / PI;  // Konwersja z radianów na stopnie
  azimuth = normalizeAngle(azimuth); 

  // Oblicz elewację (kąt w pionie)
  float accelZ = imu.a.z;
  elevation = atan2(accelZ, sqrt(accelX * accelX + accelY * accelY)) * 180.0 / PI;  // Elewacja w stopniach
  elevation = normalizeAngle(elevation); 
}

void updateRSSI(void) {
  if (rawRSSI > maxRSSI) {
    maxRSSI = rawRSSI;
    lastAzimuthMaxRSSI = azimuth;  // Zapisz azymut przy maksymalnym RSSI
  }
}

void printInfo(String str) {
    rawRSSI = readRSSI();  // odczyt RSSI bezpośrednio z Si4432

    countAngle();
    updateRSSI();

    azimuthChange = azimuth - lastAzimuthMaxRSSI;
    azimuthChange = normalizeAzimuth(azimuthChange);

    Serial.println(F("[Si4432] Received packet!"));
    const char* byteArr = str.c_str();  // zamiana na wskaźnik do tablicy bajtów
    Serial.print(F("[Si4432] Data:\t\t"));
    for (int i = 0; i < str.length(); i++) {
      Serial.print(byteArr[i], HEX);  // bajty w formie heksadecymalnej
      Serial.print(" ");
    }
    Serial.print("\n");

    Serial.print("[Si4432] RSSI raw: ");
    Serial.print(rawRSSI);
    Serial.print("\n");
    Serial.print("[LSM6DSO] azymut: ");
    Serial.print(azimuth);
    Serial.print(", elewacja: ");
    Serial.print(elevation);
    Serial.print(", zmiana Azymutu od MAX RSSI: ");
    Serial.println(azimuthChange);
    Serial.print("\n");

    drawArrow(azimuthChange);
}

void startReceive(void) {
    radio.setPacketReceivedAction(setFlag);
    Serial.print(F("[Si4432] Starting to listen ... "));

    state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
        lastPacketTime = millis();
    } 
    else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true) { delay(10); }
    }
}

void receive(void) {
  if (receivedFlag) {
      receivedFlag = false;
      // czy sygnał był wystarczająco silny
      if (!isAboveThreshold()) {
          Serial.println(F("[Si4432] RSSI below threshold, ignoring."));
          radio.startReceive();
          return;
      }

      state = radio.readData(str);

      if (state == RADIOLIB_ERR_NONE) {
          lastPacketTime = millis();
          printInfo(str);
      } 
      else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
          Serial.println(F("CRC error!"));
      } 
      else {
          Serial.print(F("failed, code "));
          Serial.println(state);
      }

      radio.startReceive();
    }
}

void displayEstimatedDistanceAndRSSI(void) {
  // Przelicz RSSI w dBm
  float rssiDbm = -120.0 + ((rawRSSI - 20.0) / 210.0) * 120.0;

  float exponent = (-68 - rssiDbm) / (10.0 * 1.4);
  // Obliczenie odległości
  float distance = 5 * pow(10, exponent);
  // Zaokrąglenie do 2 miejsc po przecinku
  distance = roundf(distance * 100) / 100.0;  
  // Konwersja odległości na tekst z jednostką "m"
  char distanceStr[20];
  snprintf(distanceStr, sizeof(distanceStr), "%.2f m", distance);

  // Ustawienie czcionki
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // Wyświetlenie odległości po lewej stronie
  u8g2.setCursor(0, 63);  
  u8g2.print(distanceStr);

  // Wyświetlenie rawRSSI i RSSI dBm po prawej stronie
  char rssiStr1[20];
  char rssiStr2[20];
  snprintf(rssiStr1, sizeof(rssiStr1), "raw: %u", rawRSSI);
  snprintf(rssiStr2, sizeof(rssiStr2), "%.1f dBm", rssiDbm);

  // Wyświetl dwie linie tekstu z prawej strony, jedna nad drugą
  uint8_t textWidth1 = u8g2.getStrWidth(rssiStr1);
  uint8_t textWidth2 = u8g2.getStrWidth(rssiStr2);
  uint8_t x1 = 128 - textWidth1;
  uint8_t x2 = 128 - textWidth2;

  u8g2.setCursor(x1, 53);  // linia wyżej
  u8g2.print(rssiStr1);

  u8g2.setCursor(x2, 63);  // dolna linia
  u8g2.print(rssiStr2);
}

void drawArrow(float azimuthChange) {
  u8g2.clearBuffer();  // Wyczyść ekran

  //- w prawo
  //+ w lewo
  if (azimuthChange < -90.0) {
    lineRight();
  } else if (azimuthChange < -20.0) {
    lineHalfRight();
  } else if (azimuthChange < 20.0) {
    lineCenter();
  } else if (azimuthChange < 90.0) {
    lineHalfLeft();
  } else {  // azimuthChange > 90.0
    lineLeft();
  }

  displayEstimatedDistanceAndRSSI();
  u8g2.sendBuffer();
}

void lineLeft() {
  u8g2.drawLine(19, 44, 64, 44);       // główna linia
  u8g2.drawLine(19, 44, 24, 41);       // strzałka góra
  u8g2.drawLine(19, 44, 24, 47);       // strzałka dół
}

void lineHalfLeft() {
  u8g2.drawLine(30, 15, 64, 44);       // główna linia
  u8g2.drawLine(30, 15, 34, 15);       // lewa górna krawędź grota
  u8g2.drawLine(30, 15, 30, 19);       // prawa dolna krawędź grota
}

void lineCenter() {
  u8g2.drawLine(64, 5, 64, 44);        // główna linia
  u8g2.drawLine(64, 5, 61, 10);        // strzałka lewa
  u8g2.drawLine(64, 5, 67, 10);        // strzałka prawa
}

void lineHalfRight() {
  u8g2.drawLine(98, 15, 64, 44);       // główna linia
  u8g2.drawLine(98, 15, 94, 15);       // lewa górna krawędź grota
  u8g2.drawLine(98, 15, 98, 19);       // prawa dolna krawędź grota
}

void lineRight() {
  u8g2.drawLine(109, 44, 64, 44);      // główna linia
  u8g2.drawLine(109, 44, 104, 41);     // strzałka góra
  u8g2.drawLine(109, 44, 104, 47);     // strzałka dół
}

void noSignal() {
  unsigned long now = millis();
  if((now - lastPacketTime) > 5000){
    u8g2.clearBuffer();  
    u8g2.setFont(u8g2_font_ncenB14_tr);
    uint8_t textWidth = u8g2.getStrWidth("no signal..."); 
    uint8_t xPos = (128 - textWidth) / 2; 
    u8g2.setCursor(xPos, 30); 
    u8g2.print("no signal..."); 
    u8g2.sendBuffer();
  }
}

void setup() {
    Serial.begin(9600);
    SPI.begin();
    Wire.begin();

    initialize();
    config();

    startReceive();
}

void loop() {
    receive();
    noSignal();
}

