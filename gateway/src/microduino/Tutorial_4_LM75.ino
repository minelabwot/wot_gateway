/**
  *这是节点侧的程序
  *模拟的是温度的测量
  *使用RTC进行定时发送
  *先按照每个资源发送一次的方式进行发送
  */
#include <Wire.h>
#include <LM75.h>

LM75 sensor(LM75_ADDRESS | 0b000);
String myStringSerial="";
int res_num=1;
int temperature_port=1;

void login()
{
  String temperature_type="temperature_zigbee";
  myStringSerial = "c:";
  myStringSerial += res_num;
  myStringSerial += ",f:0,t:";
  myStringSerial += temperature_type;
  myStringSerial += ",p:";
  myStringSerial += temperature_port;
  myStringSerial +=",s:r";
  myStringSerial += ":stt";
  Serial.println(myStringSerial);
  Serial1.print(myStringSerial);
}

void setup (void)
{
  //初始化串口
  Serial.begin(9600);
  Serial1.begin(9600);
  //初始化I2C
  Wire.begin();
  
  delay(3000);
  //节点注册
  login();
}

void loop() 
{
  send_message();
}

void send_message()
{
  int delay_time=1000*4.7;
  delay(delay_time);
  temperature();
//  delay(5000);
////  myStringSerial += ',';
//  photoresistance();
}

void temperature()
{
  myStringSerial = "p:";
  myStringSerial += temperature_port;
  myStringSerial += ",v:";
  int vol = sensor.temp();
  myStringSerial += vol;
  myStringSerial +=",s:d";
  myStringSerial += ":stt";
  if(myStringSerial!="")
  {
    Serial.println(myStringSerial); 
    Serial1.print(myStringSerial); 
  }
  myStringSerial="";
  Serial.println(sensor.temp());
}
