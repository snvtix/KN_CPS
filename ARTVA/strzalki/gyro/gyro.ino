#include <Wire.h>
#include <LSM6.h>

LSM6 imu;

float azimuth = 0.0;  // Początkowy azymut (0 stopni)
float elevation = 0.0;  // Początkowa elewacja (0 stopni)
float lastAzimuthMaxRSSI = 0.0;  // Poprzedni azymut MAX RSSI
float azimuthChange = 0.0;  // Zmiana azymutu
unsigned long lastTime = 0;  // Czas poprzedniego odczytu
float maxRSSI = 100;
float RSSI;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!imu.init()) {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
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
  RSSI = updateRSSI(lastAzimuthMaxRSSI, azimuth, maxRSSI);

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
  drawArrow(azimuthChange);

  // Zaktualizuj czas
  lastTime = currentTime;

  delay(100);  // Opóźnienie, aby nie przyspieszać za bardzo pętli
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
float updateRSSI(float &lastAzimuthMaxRSSI, float azimuth, float &maxRSSI) {
  long randomNumber = random(90, 121);
  float currentRSSI = (float)randomNumber;

  if (currentRSSI > maxRSSI) {
    maxRSSI = currentRSSI;
    lastAzimuthMaxRSSI = azimuth;  // Zapisz azymut przy maksymalnym RSSI
  }

  return currentRSSI;
}

void drawArrow(float azimuthChange){
  if(azimuthChange <= -54){
    Serial.println("==");
  }
  else if(azimuthChange > -54 && azimuthChange < -18){
    Serial.println("\\");
  }
  else if(azimuthChange >= -18 && azimuthChange <= 18 ){
    Serial.println("||");
  }
  else if(azimuthChange > 18 && azimuthChange < 54){
    Serial.println("//");
  }
  else if(azimuthChange >= 54){
    Serial.println("==");
  }
}
