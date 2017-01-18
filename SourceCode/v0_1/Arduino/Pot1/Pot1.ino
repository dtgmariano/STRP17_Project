const int numPots = 1;
int iPots[numPots];
int counter = 0;

void setup() {
  Serial.begin(9600);
}

void loop() 
{
  readData();
  sendData();
}

void readData()
{
  for(int i=0; i<numPots; i++)
  {
    iPots[i] = analogRead(i);
    delay(10);
  }

  counter++;
  if(counter>=1024);
    counter = 0;
}

void sendData()
{
  for(int i=0; i<numPots; i++)
  {
    Serial.println(iPots[i]);
    delay(50);
  }

  //Serial.println(counter);
}

