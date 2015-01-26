/**
  *这是网关microduino侧的程序
  */
String myStringSerial="stt:,m:";
String myStringSerial1="";
char* myStringSerial_down="";
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
  if(Serial.available()>0)
  {
    myStringSerial_down += char(Serial.read());
    if(strstr(myStringSerial_down,"+Z")||strstr(myStringSerial_down,"+B"))
    {
      delay(2);
      while (Serial.available() > 0)  
      {
        myStringSerial_down += char(Serial.read());
        delay(2);
      }
      if (strlen(myStringSerial_down)> 0)
      {
        Serial1.print(myStringSerial_down);
        myStringSerial_down="";
      }
    }
    else
    {
      myStringSerial_down="";
    }
  }
}


