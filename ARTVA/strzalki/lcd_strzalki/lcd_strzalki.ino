/*
  Arduino TFT text example

  This example demonstrates how to draw text on the
  TFT with an Arduino. The Arduino reads the value
  of an analog sensor attached to pin A0, and writes
  the value to the LCD screen, updating every
  quarter second.

  This example code is in the public domain

  Created 15 April 2013 by Scott Fitzgerald

  https://www.arduino.cc/en/Tutorial/LibraryExamples/TFTDisplayText

 */

#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// pin definitions for the Uno
#define cs   10
#define dc   9
#define rst  8

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

// char array to print to the screen
char sensorPrintout[8];

void setup() {
  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();
  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(255, 255, 255);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.text("RSSI:\n ", 0, 5);
  // set the font size very large for the loop
  TFTscreen.setTextSize(3);
}

void loop() {

  // Read the value of the sensor on A0
  String sensorVal = "-91 dBm";

  // convert the reading to a char array
  sensorVal.toCharArray(sensorPrintout, 8);

  // set the font color
  TFTscreen.stroke(255, 255, 255);
  // print the sensor value
  TFTscreen.text(sensorPrintout, 0, 25);
  lineLeft();
  lineHalfLeft();
  lineCenter();
  lineHalfRight();
  lineRight();
  // wait for a moment
  delay(250);
  // erase the text you just wrote
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(sensorPrintout, 0, 20);
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