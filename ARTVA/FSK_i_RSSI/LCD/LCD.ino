#include <Wire.h>
#include <U8g2lib.h>

// Dla ESP8266 i SH1106 – I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  u8g2.begin();
  Serial.begin(9600);

  // Rysujemy raz przy starcie
  rysujStrzalki();
}

void loop() {
  Serial.println("Wyswietlacz gotowy");
  delay(2000);
}

void rysujStrzalki() {
  u8g2.clearBuffer();
  lineLeft();
  lineHalfLeft();
  lineCenter();
  lineHalfRight();
  lineRight();
  u8g2.sendBuffer();
}

void lineLeft() {
  u8g2.drawLine(19, 44, 64, 44);       // główna linia, prowadzi do środka (jak inne)
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


