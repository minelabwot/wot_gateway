/**
  *这是网关microduino侧的程序
  */
#include <string>
#include <SoftwareSerial.h>
SoftwareSerial mySerial1(4,5);
const int mac_size=100;

String myStringSerial="stt:,m:";
String myStringSerial1="";
String myStringSerial_down="";
String myStringSerial_flag="";
String myStringSerial1_temp="";

//本地MAC列表
String mac_message[mac_size];
String mac_address[mac_size];
unsigned long start[mac_size];
int mac_address_num=0;

void setup()  
{
  Serial.begin(115200);
  mySerial1.begin(9600);
//  Serial1.begin(9600);
}

void loop() // run over and over
{
  package_serial();
  heart_check();
}

void heart_check()
{
  for(int i=mac_address_num;i>=0;i--)
  {
    if(mac_address[i]!="" && (millis()-start[i])>3600 && mac_message[i]!="")
    {
//      Serial.println(mac_address[i]);
//      Serial.println(i);
      mac_message[i][16]='1';
      myStringSerial += mac_message[i];
      Serial.println(myStringSerial);
      mac_message[i]="";
      start[i]=0;
      mac_address[i]="";
//      Serial.println("显示当前数据信息：");
//      for(int k=0;k<mac_address_num;k++)
//      {
//        Serial.println(start[k]);
//        Serial.println(mac_address[k]);
//        Serial.println(mac_message[k]);
//      }
      myStringSerial="stt:,m:";
    }
  }
}
      

void package_serial()
{
  int loops=0;
  String mac_address_receive="";
//  while (Serial1.available() > 0)  
//  {
//    myStringSerial1 += char(Serial1.read());
//    loops++;
//    if(loops==9) mac_address_receive=myStringSerial1;
//    delay(2);
//  }
  while (mySerial1.available() > 0)  
  {
    myStringSerial1 += char(mySerial1.read());
    loops++;
    if(loops==9 && myStringSerial1[0]=='+') mac_address_receive=myStringSerial1;
    delay(2);
  }
  
  int serial_len=myStringSerial1.length();
  if(myStringSerial1[serial_len-1]=='p')
  {
    myStringSerial1_temp=myStringSerial1;
    myStringSerial1="";
  }
  if(myStringSerial1[0]==':')
  {
    myStringSerial1=myStringSerial1_temp+myStringSerial1;
  }
//  if(myStringSerial1.length()>=4)
//  Serial.println(myStringSerial1);
  
  //心跳检测
  if(myStringSerial1.length()>8)
  {
    if(myStringSerial1[14]=='f')
    {
      mac_message[mac_address_num]=myStringSerial1;
      mac_address[mac_address_num]=mac_address_receive;
      start[mac_address_num]=millis();
//      Serial.print("mac_address_receive:");
//      Serial.println(mac_address_receive);
//      Serial.println(mac_message[mac_address_num]);
//      Serial.println(mac_address[mac_address_num]);
//      Serial.println(start[mac_address_num]);
      mac_address_num++;
    }
    else
    {
      for(int i=0;i<mac_address_num;i++)
      {
//        Serial.print("loops=");
//        Serial.println(i);
        if(mac_address[i]==mac_address_receive)
        {
          start[i]=millis();
//          Serial.println("显示当前数据信息：");
//          for(int k=0;k<mac_address_num;k++)
//          {
//            Serial.print("k=");
//            Serial.println(k);
//            Serial.println(start[k]);
//            Serial.println(mac_address[k]);
//            Serial.println(mac_message[k]);
//          }
//          Serial.println(start[i]);
//          Serial.println(i);
          break;
        }
      }
    }
  }
  
  if (myStringSerial1.length() > 23 && (myStringSerial1[10] == 'c' || myStringSerial1[10] == 'p'))
//  if(myStringSerial1.length() >23)
  { 
    myStringSerial += myStringSerial1;
    Serial.println(myStringSerial);
  }
  myStringSerial="stt:,m:";
  myStringSerial1="";
}

void serialEvent()
{
  while(Serial.available()>0)
  {
    myStringSerial_down += char(Serial.read());
    delay(2);
  }
//    myStringSerial_down = "+ZBD=1983,on";
  for(int i=0;i<myStringSerial_down.length();i++)
  {
//    if( myStringSerial_down[i]=='+'&& myStringSerial_down[i+1]=='Z')
//    {
//      myStringSerial_down += "\r\n";
//      Serial1.println(myStringSerial_down);
////      Serial.print(myStringSerial_down);
//      break;
//    }
    if( myStringSerial_down[i]=='+'&& myStringSerial_down[i+1]=='B')
    {
      myStringSerial_down += "\r\n";
      mySerial1.println(myStringSerial_down);
//      Serial.print(myStringSerial_down);
      break;
    }
  }
  myStringSerial_down="";
}
