#include <Wire.h>
#include <LSM6.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// pin definitions for the Uno
#define cs   10
#define dc   9
#define rst  8

LSM6 imu;
TFT TFTscreen = TFT(cs, dc, rst);

float azimuth = 0.0;  // Początkowy azymut (0 stopni)
float elevation = 0.0;  // Początkowa elewacja (0 stopni)
float lastAzimuthMaxRSSI = 0.0;  // Poprzedni azymut MAX RSSI
float azimuthChange = 0.0;  // Zmiana azymutu
unsigned long lastTime = 0;  // Czas poprzedniego odczytu
float maxRSSI = 95;
float RSSI;
int numRSSI = 0;
String stringRSSI;
char sensorPrintout[8];

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!imu.init()) {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();

  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255, 255, 255);
}

void loop() {
  imu.read();

  // Oblicz czas, który minął od ostatniego odczytu
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0;  // czas w sekundach

  // Oblicz azymut na podstawie przyspieszenia (akcelerometr)
  float accelX = imu.a.x;
  float accelY = imu.a.y;
  azimuth = atan2(accelY, accelX) * 180.0 / PI;  // Konwersja z radianów na stopnie
  azimuth = normalizeAngle(azimuth);  // Ogranicz azymut do -180° do 180°

  // Oblicz elewację (kąt w pionie)
  float accelZ = imu.a.z;
  elevation = atan2(accelZ, sqrt(accelX * accelX + accelY * accelY)) * 180.0 / PI;  // Elewacja w stopniach
  elevation = normalizeAngle(elevation);  // Ogranicz elewację do -180° do 180°

  // Oblicz RSSI
  RSSI = updateRSSI(lastAzimuthMaxRSSI, azimuth, maxRSSI, numRSSI);

  // Oblicz zmianę azymutu
  azimuthChange = azimuth - lastAzimuthMaxRSSI;

  // Wyświetl kąt i azymut na monitorze szeregowym
  Serial.print("Azymut: ");
  Serial.print(azimuth);
  Serial.print("  Elewacja: ");
  Serial.print(elevation);
  Serial.print("  RSSI: -");
  Serial.print(RSSI);
  Serial.print(" dBm  Zmiana Azymutu od pozycji MAX RSSI: ");
  Serial.println(azimuthChange);

  stringRSSI = (String)RSSI;
  // convert the reading to a char array
  stringRSSI.toCharArray(sensorPrintout, 8);
  // set the font color
  TFTscreen.stroke(255, 255, 255);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.text("RSSI:\n ", 0, 5);
  // set the font size very large for the loop
  TFTscreen.setTextSize(3);
  // print the sensor value
  TFTscreen.text(sensorPrintout, 0, 25);
  drawArrow(azimuthChange);
  
  delay(1000);  // Opóźnienie, aby nie przyspieszać za bardzo pętli
  TFTscreen.background(0,0,0); 
  // erase the text you just wrote
  lastTime = currentTime;
}

// Funkcja ograniczająca kąt do zakresu -180° do 180°
float normalizeAngle(float angle) {
  if (angle > 180.0) {
    angle -= 360.0;
  } else if (angle < -180.0) {
    angle += 360.0;
  }
  return angle;
}

// Funkcja do obliczania RSSI
float updateRSSI(float &lastAzimuthMaxRSSI, float azimuth, float &maxRSSI, int &numRSSI) {
  long randomNumber = 100;
  float currentRSSI;
  if(numRSSI == 0){
    currentRSSI = maxRSSI;
    lastAzimuthMaxRSSI = azimuth;  // Zapisz azymut przy maksymalnym RSSI
    numRSSI = 1;
  }else{
    currentRSSI = randomNumber;
  }

  /*
  if (currentRSSI > maxRSSI) {
    maxRSSI = currentRSSI;
    lastAzimuthMaxRSSI = azimuth;  // Zapisz azymut przy maksymalnym RSSI
  }
  */

  return currentRSSI;
}

void drawArrow(float azimuthChange){
  if(azimuthChange <= -54){
    Serial.println("<-54");
    lineLeft();
  }
  else if(azimuthChange > -54 && azimuthChange < -18){
    Serial.println("-54 - -18");
    lineHalfLeft();
  }
  else if(azimuthChange >= -18 && azimuthChange <= 18 ){
    Serial.println("-18 - 18");
    lineCenter();
  }
  else if(azimuthChange > 18 && azimuthChange < 54){
    Serial.println("18 - 54");
    lineHalfRight();
  }
  else if(azimuthChange >= 54){
    Serial.println(">54");
    lineRight();
  }
}

void lineLeft(){
  TFTscreen.line(35, 109, 40, 104);
  TFTscreen.line(35, 110, 40, 105);
  TFTscreen.line(35, 111, 40, 106);
  //
  TFTscreen.line(35, 109, 80, 109);
  TFTscreen.line(35, 110, 80, 110);
  TFTscreen.line(35, 111, 80, 111);
  //
  TFTscreen.line(35, 109, 40, 114);
  TFTscreen.line(35, 110, 40, 115);
  TFTscreen.line(35, 111, 40, 116);
}

void lineHalfLeft(){
  TFTscreen.line(46, 74, 46, 83); 
  TFTscreen.line(45, 75, 45, 84);
  TFTscreen.line(44, 76, 44, 85);
  //
  TFTscreen.line(46, 74, 81, 109); 
  TFTscreen.line(45, 75, 80, 110);
  TFTscreen.line(44, 76, 79, 111);
  //
  TFTscreen.line(46, 74, 55, 74); 
  TFTscreen.line(45, 75, 54, 75);
  TFTscreen.line(44, 76, 53, 76);
}

void lineCenter(){
  TFTscreen.line(79, 65, 74, 70);
  TFTscreen.line(80, 65, 75, 70); 
  TFTscreen.line(81, 65, 76, 70);
  //
  TFTscreen.line(79, 65, 79, 110);
  TFTscreen.line(80, 65, 80, 110); 
  TFTscreen.line(81, 65, 81, 110);
  //
  TFTscreen.line(79, 65, 84, 70);
  TFTscreen.line(80, 65, 85, 70); 
  TFTscreen.line(81, 65, 86, 70);
}

void lineHalfRight(){
  TFTscreen.line(114, 74, 114, 83);
  TFTscreen.line(115, 75, 115, 84);
  TFTscreen.line(116, 76, 116, 85);
  //
  TFTscreen.line(114, 74, 79, 109);
  TFTscreen.line(115, 75, 80, 110);
  TFTscreen.line(116, 76, 81, 111);
  //
  TFTscreen.line(114, 74, 105, 74);
  TFTscreen.line(115, 75, 106, 75);
  TFTscreen.line(116, 76, 107, 76);
}

void lineRight(){
  TFTscreen.line(125, 109, 120, 104);
  TFTscreen.line(125, 110, 120, 105);
  TFTscreen.line(125, 111, 120, 106);
  //
  TFTscreen.line(125, 109, 80, 109);
  TFTscreen.line(125, 110, 80, 110);
  TFTscreen.line(125, 111, 80, 111);
  //
  TFTscreen.line(125, 109, 120, 114);
  TFTscreen.line(125, 110, 120, 115);
  TFTscreen.line(125, 111, 120, 116);
}