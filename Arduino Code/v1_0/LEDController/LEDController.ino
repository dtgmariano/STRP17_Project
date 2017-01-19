const int sizeVector = 2;
int input[sizeVector];

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
  /*
   * doSerialReading checks if there is any data available and then atributtes the values to vector value
   * Data ranges from 0 to 1023 for each variable
   * 
   */
  doSerialReading();

  /*
   * Here you can enter your functions to control lights
   * 
   */
  
  delay(10);
}

void doSerialReading()
{
  if(Serial.available())
  {
    for(int i=0; i< sizeVector; i++)
      input[i] = Serial.read();
      
    digitalWrite(LED4, HIGH);
  }
}

