/*
 * An IR LED must be connected to Arduino PWM pin 3.
 * Serial.input "p" to take photo.
 */

#include <IRremote.h>
#include <Wire.h>

IRsend irsend;

void setup()
{
  Serial.begin(9600);
}

void loop() 
{
  if(Serial.available()) 
  {
    if(Serial.read()=='p')
    {
      for (int i = 0; i < 3; i++) 
      {
        irsend.sendSony(0xB4B8F, 20); // Sony TV power code
        delay(12);
      }
      Serial.println("take photo");    
    }
  }
}
