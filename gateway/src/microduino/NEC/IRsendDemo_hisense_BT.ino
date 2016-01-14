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

long long heart_start=millis();

void login()
{
  
  //节点注册信息设置
  int res_num=1;
  int IR_port=1;
  String IR_type="TV";
  String myStringSerial = "+BBD=ACBD,c:";
  myStringSerial += res_num;
  myStringSerial += ",f:0,p:";
  myStringSerial += IR_port;
  myStringSerial += ",t:";
  myStringSerial += IR_type;
  myStringSerial +=",s:r";
  myStringSerial += ":stt";
  Serial.println(myStringSerial);
  mySerial.print(myStringSerial);
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
 
void loop() // run over and over
{
  package_serial();
//  test();
  if(millis()-heart_start>1700)
  {
    heart_check();
    heart_start=millis();
  }
}

void heart_check()
{
  String myStringSerial = "+BBD=ACBD,heart";
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

void package_analysis()
{
    for(int i=0;i<myStringSerial_down.length();i++)
    {
      if(myStringSerial_down[i]=='o'&&myStringSerial_down[i+1]=='n')
      {
        Serial.println("on");
        irsend.sendNEC(0xFDB04F, 32);   // kaiji
//        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
      }
      else if(myStringSerial_down[i]=='u'&&myStringSerial_down[i+1]=='p')
      {
        Serial.println("voice_up");
        irsend.sendNEC(0xFD52AD, 32);   // kaiji
//        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
        delay(20);
        irsend.sendNEC(0xFD22DD, 32);   // kaiji
//        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
      }
      else if(myStringSerial_down[i]=='d'&&myStringSerial_down[i+1]=='o')
      {
        Serial.println("voice_down");
        irsend.sendNEC(0xFDC23D, 32);   // kaiji
//        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
        delay(20);
        irsend.sendNEC(0xFDC23D, 32);   // kaiji
//        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
      }
      else if(myStringSerial_down[i]=='l'&&myStringSerial_down[i+1]=='e')
      {        
        Serial.println("channal_reduice");
        irsend.sendNEC(0xFDD22D, 32);   // kaiji
//        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
      }
      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
      {
        Serial.println("channal_add");
        irsend.sendNEC(0xFD52AD, 32);   // kaiji

//        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("up");
//        irsend.sendNEC(0xFD6897, 32);   // kaiji
//        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("down");
//        irsend.sendNEC(0xFDE817, 32);   // kaiji
////        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("left");
//        irsend.sendNEC(0xFD9867, 32);   // kaiji
////        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("right");
//        irsend.sendNEC(0xFD18E7, 32);   // kaiji
////        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("ok");
//        irsend.sendNEC(0xFDA857, 32);   // kaiji
////        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("silence");
//        irsend.sendNEC(0xFD708F, 32);   // kaiji
////        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("ok");
//        irsend.sendNEC(0xFDA857, 32);   // kaiji
////        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("ok");
//        irsend.sendNEC(0xFDA857, 32);   // kaiji
////        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("ok");
//        irsend.sendNEC(0xFDA857, 32);   // kaiji
////        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
//      else if(myStringSerial_down[i]=='r'&&myStringSerial_down[i+1]=='i')
//      {
//        Serial.println("ok");
//        irsend.sendNEC(0xFDA857, 32);   // kaiji
////        irsend.sendNEC(0xFFFFFFFF, 64);   // kaiji
//      }
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

