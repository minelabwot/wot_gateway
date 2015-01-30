/**
  *这是网关microduino侧的程序
  */
String myStringSerial="stt:,m:";
String myStringSerial1="";
String myStringSerial_down="";
void setup()  
{
  Serial.begin(115200);
  Serial1.begin(9600);
}
 
void package_serial()
{
  while (Serial1.available() > 0)  
  {
    myStringSerial1 += char(Serial1.read());
    delay(2);
  }
  if (myStringSerial1.length() > 0)
  {   
    myStringSerial += myStringSerial1;
    Serial.print(myStringSerial);
    myStringSerial="stt:,m:";
    myStringSerial1="";
  }
}
void loop() // run over and over
{
  package_serial();
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
