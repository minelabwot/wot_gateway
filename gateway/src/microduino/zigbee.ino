/**
  *这是节点侧的程序
  *模拟的是温度的测量
  *使用RTC进行定时发送
  *先按照每个资源发送一次的方式进行发送
  */
int send_message_num=0;

String myStringSerial="";
int res_num=2;
int temperature_port=1;
int photoresistance_port=2;
String temperature_type="temperature";
String photoresistance_type="photoresistance";

void login()
{
  myStringSerial = "c:";
  myStringSerial += res_num;
  myStringSerial += ",f:0,p:";
  myStringSerial += temperature_port;
  myStringSerial += ",t:";
  myStringSerial += temperature_type;
  myStringSerial += ",p:";
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
  Serial.begin(9600);
  Serial1.begin(9600);
  
  //节点注册
  login();
}
 
void loop() // run over and over
{

  send_message();
  
//  if(send_message_flag)
//  {
//    Serial.print("Send message sucessfully.Have send ");
//    Serial.print(send_message_num);
//    Serial.println("datas");
//  }
}

void send_message()
{
  int delay_time=1000*20;
  delay(delay_time);
  temperature();
  delay(3000);
//  myStringSerial += ',';
  photoresistance();
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
