/**
  *这是节点侧的程序
  *模拟的是温度的测量
  *使用RTC进行定时发送
  *先按照每个资源发送一次的方式进行发送
  */
#include <Rtc_Pcf8563.h>
#include <Wire.h>

boolean send_message_flag=LOW;
int send_message_num=0;

String myStringSerial="";
Rtc_Pcf8563 RTC;
int res_num=2;
int temperature_port=1;
int photoresistance_port=2;

void login()
{
  myStringSerial = "c:";
  myStringSerial += res_num;
  myStringSerial += ",f:0,p:";
  myStringSerial += temperature_port;
  myStringSerial += ",p:";
  myStringSerial += photoresistance_port;
  myStringSerial +=",s:r";
  myStringSerial += ":stt";
  Serial.println(myStringSerial);
  Serial1.print(myStringSerial);
}

void setup()  
{
  Serial.begin(9600);
  Serial1.begin(9600);
  
//  //RTC时钟控制
//  //clear out all the registers
//  RTC.initClock();
//  //set a time to start with.
//  //day, weekday, month, century, year
//  RTC.setDate(14, 6, 3, 0, 10);
//  //hr, min, sec
//  RTC.setTime(1, 15, 40);
  
  //节点注册
  login();
}
 
void loop() // run over and over
{
//  int time=RTC.getSecond();
//  if(time==20)
//  {
//    Serial.println(time);
//    rtc();
//    temperature();
//    photoresistance();
//  }

  send_message();
  
//  if(send_message_flag)
//  {
//    Serial.print("Send message sucessfully.Have send ");
//    Serial.print(send_message_num);
//    Serial.println("datas");
//  }
}

void rtc()
{
  //调用时间，不能缺少
  RTC.formatDate();
  RTC.formatTime();
  
  Serial.println(RTC.getSecond());
}

void send_message()
{
  int delay_time=1000*20;
  delay(delay_time);
  temperature();
  delay(3000);
//  myStringSerial += ',';
  photoresistance();
  send_message_flag=!send_message_flag;
}

void temperature()
{
  int temp=analogRead(A0);
  int vol=temp*(5.0/1023.0*1000/10);
  myStringSerial = "p:";
  myStringSerial += temperature_port;
  myStringSerial += ",v:";
  myStringSerial += vol;
  myStringSerial +=",s:d";
  myStringSerial += ":stt";
  if(myStringSerial!="")
  {
    Serial.println(myStringSerial); 
    Serial1.print(myStringSerial); 
  }
  myStringSerial="";
}

void photoresistance()
{
  int temp=analogRead(A1);
//  myStringSerial += "p:1,v:";  
  myStringSerial = "p:";
  myStringSerial += photoresistance_port;
  myStringSerial += ",v:";
  myStringSerial += temp;
  myStringSerial +=",s:d";
  myStringSerial += ":stt";
  if(myStringSerial!="")
  {
    Serial.println(myStringSerial); 
    Serial1.print(myStringSerial); 
  }
  myStringSerial="";
}
