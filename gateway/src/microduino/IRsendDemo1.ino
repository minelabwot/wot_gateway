/*
* 发射红外线信号
* 注意! 红外线发射器(IR LED)必须接到 pin 3, 不能接其它脚位
 
 Hisense 机顶盒 SD-16127 tested
 */
 
//By Leo 2013.02.19
//http://weibo.com/leolite
//Update: Pin 3 IR LED test function
 
 
#include <IRremote.h>                  // 引用 IRRemote 函ZXCXCCVV?式库
#include <SoftwareSerial.h>

SoftwareSerial mySerial(4,5); // RX, TX

IRsend irsend;                          // 定义 IRsend 物件来发射红外线讯号
String myStringSerial_down="";

void setup()  
{
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(4,INPUT);
  pinMode(5,OUTPUT);
}
 
void loop() // run over and over
{
  package_serial();
//  test();
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

void package_analysis()
{
    for(int i=0;i<myStringSerial_down.length();i++)
    {
      if(myStringSerial_down[i]=='o'&&myStringSerial_down[i+1]=='n')
      {
        Serial.println("on");
        irsend.sendNEC(0xFDB04F, 32);   // kaiji
      }
      else if(myStringSerial_down[i]=='u'&&myStringSerial_down[i+1]=='p')
      {
        Serial.println("voice_up");
        irsend.sendNEC(0xFD22DD, 32);   // kaiji
      }
      else if(myStringSerial_down[i]=='d'&&myStringSerial_down[i+1]=='o')
      {
        Serial.println("voice_down");
        irsend.sendNEC(0xFDC23D, 32);   // kaiji
      }
    }
}


void test()
{
  if(digitalRead(7)==HIGH)
  {
    delay(20);
    if(digitalRead(7)==HIGH)
    {
      Serial.println("ON");
      irsend.sendNEC(0xFDB04F, 32);   // kaiji
    }
    while(digitalRead(7)==HIGH);
  }
  else if(digitalRead(8)==HIGH)
  {
    delay(20);
    if(digitalRead(8)==HIGH)
    {
      Serial.println("voice_up");
      irsend.sendNEC(0xFD22DD, 32);   // kaiji
    }
    while(digitalRead(8)==HIGH);
  }  else if(digitalRead(9)==HIGH)
  {
    delay(20);
    if(digitalRead(9)==HIGH)
    {
      Serial.println("voice_down");
      irsend.sendNEC(0xFDC23D, 32);   // kaiji
    }
    while(digitalRead(9)==HIGH);
  }
}

