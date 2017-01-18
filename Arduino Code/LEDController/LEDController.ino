const int sizeVector = 6;
int value[sizeVector];
const int LED1 = 2;
const int LED2 = 3;
const int LED3 = 4;
const int LED4 = 5;
const int MSDURATION = 500;
const int X1 = 100;
const int X2 = 200;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  Serial.begin(9600);
}

void loop() 
{
  if(Serial.available())
  {
    for(int i=0; i< sizeVector; i++)
      value[i] = Serial.read();
      
    digitalWrite(LED4, HIGH);
  }

  if(value[0] <= X1 && 
     value[1] <= X1 && 
     value[2] <= X1 && 
     value[3] <= X1 && 
     value[4] <= X1 && 
     value[5] <= X1  
     )
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    //digitalWrite(LED4, LOW);
    delay(MSDURATION);
    digitalWrite(LED4, LOW);
    digitalWrite(LED2, HIGH);
    delay(MSDURATION);
  }
  else if(value[0] <= X2 && 
          value[1] <= X2 && 
          value[2] <= X2 && 
          value[3] <= X2 && 
          value[4] <= X2 && 
          value[5] <= X2 
     )
  {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    //digitalWrite(LED4, LOW);
    delay(MSDURATION);
    digitalWrite(LED4, LOW);
    digitalWrite(LED3, HIGH);
    delay(MSDURATION);
  }
  else
  {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
  }
  
  delay(10);
}
