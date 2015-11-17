/**
  *这是节点侧的程序
  *模拟的是温度的测量
  *使用RTC进行定时发送
  *先按照每个资源发送一次的方式进行发送
  */

String myStringSerial="";


void setup()  
{
  Serial.begin(115200);
  delay(3000);
}
 
void loop() // run over and over
{
  send_message();
}

void send_message()
{
  int delay_time=1000*4.7;
  delay(delay_time);  
  photoresistance();
}

void photoresistance()
{
  int myStringSerial=analogRead(A0);
  Serial.println(myStringSerial); 
}
