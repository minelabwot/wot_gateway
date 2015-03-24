/**
  *这是节点侧的程序
  *模拟的是温度的测量
  *使用RTC进行定时发送
  *先按照每个资源发送一次的方式进行发送
  */
int send_message_num=0;

String myStringSerial="";
int res_num=1;
//int temperature_port=1;
int photoresistance_port=1;
//String temperature_type="temperature";
String photoresistance_type="light";

void login()
{
  myStringSerial = "c:";
  myStringSerial += res_num;
  myStringSerial += ",f:0,p:";
  myStringSerial += photoresistance_port;
  myStringSerial += ",t:";
  myStringSerial += photoresistance_type;
  myStringSerial +=",s:r";
  myStringSerial += ":stt";
  Serial.println(myStringSerial);
  Serial1.print(myStringSerial);
}

void setup()  
{
  Serial.begin(115200);
  Serial1.begin(9600);
  delay(3000);
  //节点注册
  login();
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
  int light_temp=analogRead(A0);
  myStringSerial = "p:";
  myStringSerial += photoresistance_port;
  myStringSerial += ",v:";
  myStringSerial += light_temp;
  myStringSerial +=",s:d";
  myStringSerial += ":stt";
  if(myStringSerial!="")
  {
    Serial.println(myStringSerial); 
    Serial1.print(myStringSerial); 
  }
  myStringSerial="";
}
