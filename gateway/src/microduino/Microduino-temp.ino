/**
  *这是节点侧的程序
  *模拟的是温度的测量
  *使用RTC进行定时发送
  *先按照每个资源发送一次的方式进行发送
  */
#include <Wire.h>
#include <LM75.h>

LM75 sensor(LM75_ADDRESS | 0b000);

void setup (void)
{
  //初始化串口
  Serial.begin(115200);
  //初始化I2C
  Wire.begin();  
  delay(3000);
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
}

void temperature()
{
  Serial.println(sensor.temp());
}
