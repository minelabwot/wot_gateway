/**
  *这是网关microduino侧的程序
  */
#include<String>
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial1(2,3);

String myStringSerial="stt:,m:";
String myStringSerial1="";
String myStringSerial_down="";
String myStringSerial_flag="";

//本地MAC列表
String mac_message[5];
String mac_address[5];
long long start[5]={0,0,0,0,0};
int mac_address_num=0;

void setup()  
{
  Serial.begin(115200);
//  mySerial1.begin(9600);
  Serial1.begin(9600);
}

void loop() // run over and over
{
  package_serial();
  heart_check();
}

void heart_check()
{
  for(int i=0;i<5;i++)
  {
    if((millis()-start[i])>120000 && start[i]!=0)
    {
      mac_message[i][16]='1';
      myStringSerial += mac_message[i];
      Serial.print(myStringSerial);
      mac_message[i]="";
      start[i]=0;
      mac_address[i]="";
      myStringSerial="stt:,m:";
    }
  }
}
      

void package_serial()
{
  while (Serial1.available() > 0)  
  {
    myStringSerial1 += char(Serial1.read());
    delay(2);
  }
  
  //心跳检测
  String mac_address_receive=myStringSerial1;
  mac_address_receive[9]='\0';
  if(myStringSerial1[14]=='f')
  {
    mac_message[mac_address_num]=myStringSerial1;
    mac_address[mac_address_num]=mac_address_receive;
    mac_address_num++;
  }
  else
  {
    for(int i=0;i<5;i++)
    {
      if(mac_address[i]==mac_address_receive)
      {
        start[i]=millis();
        break;
      }
    }
  }
  
  if (myStringSerial1.length() > 0)
  {   
    myStringSerial += myStringSerial1;
    Serial.print(myStringSerial);
    myStringSerial="stt:,m:";
    myStringSerial1="";
  }
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
    if( myStringSerial_down[i]=='+'&& myStringSerial_down[i+1]=='Z')
    {
      myStringSerial_down += "\r\n";
      Serial1.print(myStringSerial_down);
      Serial.print(myStringSerial_down);
      break;
    }
  }
  myStringSerial_down="";
}
