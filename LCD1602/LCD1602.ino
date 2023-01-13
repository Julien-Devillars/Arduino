#include "LiquidCrystal.h"
LiquidCrystal LCD(4, 6, 10, 11, 12, 13);

/*

GND 5V  5V 4  GND 6             10 11 12 13  3.3 GND     
|    |   | |   |  |  |  |  |  |  |  |  |  |  |   |
VSS VDD V0 RS RW  E D0 D1 D2 D3 D4 D5 D6 D7  A   K

V0 => Can be connected to a potentiometer
*/

void setup() {
   LCD.begin(16, 2); // Define the LLD screen to 16 x 2

   LCD.setCursor(1, 0);
   LCD.print("Hello");
  
   LCD.setCursor(8, 1);
   LCD.print("World");
}
 
void loop() 
{
  delay(1000);
}