#include <Adafruit_SI5351.h>

Adafruit_SI5351 clockgen = Adafruit_SI5351();

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Si5351 Clockgen Test"); Serial.println("");

  /* Initialise the sensor */
  if (clockgen.begin() != ERROR_NONE)
  {
    /* There was a problem detecting the IC ... check your connections */
    Serial.print("Ooops, no Si5351 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  Serial.println("OK!");

  // https://learn.adafruit.com/adafruit-si5351-clock-generator-breakout/wiring-and-test

  // 600.041 MHz 
  clockgen.setupPLL(SI5351_PLL_A, 24, 164, 100000); //
  Serial.println("Set Output #1 to 457 kHz");
  clockgen.setupMultisynth(1, SI5351_PLL_A, 900, 413, 1); // I 4 - 900, II 0 - 1048575, III 1 - 1048575

  /* Enable the clocks */
  clockgen.enableOutputs(true);
}

void loop(void)
{

}