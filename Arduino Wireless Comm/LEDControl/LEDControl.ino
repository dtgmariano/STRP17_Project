int value;
bool flag = false;
const int LED1 = 2;
const int LED2 = 3;
const int LED3 = 4;
const int LED4 = 5;
const int MSDURATION = 500;

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
    value = Serial.read();
    digitalWrite(LED4, HIGH);
  }

  if(value == 23)
  {
    flag = true;
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    //digitalWrite(LED4, LOW);
    delay(MSDURATION);
    digitalWrite(LED4, LOW);
    digitalWrite(LED2, HIGH);
    delay(MSDURATION);
  }
  else if(value ==10)
  {
    flag = false;
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
