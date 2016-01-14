/*
 * An IR LED must be connected to Arduino PWM pin 3.
 * Serial.input "p" to take photo.
 */

#include <IRremote.h>
#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4,5); // RX, TX

IRsend irsend;
String myStringSerial_down="";

long long heart_start=millis();

void login()
{
  
  //节点注册信息设置
  int res_num=1;
  int IR_port=1;
  String IR_type="camera";
//  String IR_name="01times2camera";
  String myStringSerial = "+BCD=ABCD,c:";
  myStringSerial += res_num;
  myStringSerial += ",f:0,p:";
  myStringSerial += IR_port;
  myStringSerial += ",t:";
  myStringSerial += IR_type;
//  myStringSerial += ",n:";
//  myStringSerial += IR_name;
  myStringSerial +=",s:r";
  myStringSerial += ":stt";
  Serial.println(myStringSerial);
  mySerial.println(myStringSerial);
}

void setup()  
{
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(4,INPUT);
  pinMode(5,OUTPUT);
  
  delay(6000);
  //节点注册
  login();
}

void loop() 
{
  package_serial();
//  test();
  if(millis()-heart_start>1500)
  {
    heart_check();
    heart_start=millis();
  }
//    irsend.sendSony(0xB4B8F, 20); // Sony TV power code
//
//  Serial.println("take photo");    
}

void heart_check()
{
  String myStringSerial = "+BCD=ABCD,heart";
  Serial.println(myStringSerial);
  mySerial.print(myStringSerial);
}

void package_serial()
{
  if(mySerial.available()>0)
  {
    while (mySerial.available() > 0)  
    {
      myStringSerial_down += char(mySerial.read());
      delay(2);
    }
    if (myStringSerial_down.length() > 0)
    {   
      Serial.print(myStringSerial_down);
      package_analysis();
      myStringSerial_down="";
    }
  }
}
//
//void package_serial()
//{
//  if(Serial.available()>0)
//  {
//    while (Serial.available() > 0)  
//    {
//      myStringSerial_down += char(Serial.read());
//      delay(2);
//    }
//    if (myStringSerial_down.length() > 0)
//    {   
//      Serial.print(myStringSerial_down);
//      package_analysis();
//      myStringSerial_down="";
//    }
//  }
//}

void package_analysis()
{
    for(int i=0;i<myStringSerial_down.length();i++)
    {
      if(myStringSerial_down[i]=='o'&&myStringSerial_down[i+1]=='n')
      {
        Serial.println("on");
        for (int i = 0; i < 3; i++) 
        {
          irsend.sendSony(0xB4B8F, 20); // Sony TV power code
          delay(12);
        }
      }
    }
}

