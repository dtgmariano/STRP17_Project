/******************************************************/
/*Project: Arduino + Accelerometer + Sound + Lights   */
/* Author: Daniel Mariano                             */
/*Version: 0.1                                        */
/*   Date: 11/01/2017                                 */
/******************************************************/

/*Declaring Variables*/
int val1 = 0;
int val2 = 0;
int val3 = 0;
int i = 0;

/*Setup*/
void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
}

void loop()
{
  sawtoothwave();
}

void sawtoothwave()
{
  val1++;
  if(val1>=30)
    val1 = 0;
  Serial.print(val1, DEC);
  Serial.print(" ");
  Serial.print(val1, DEC);
  Serial.print(" ");
  Serial.print(val1, DEC);
  Serial.print(" ");
  Serial.println();
  Serial.print("\r");
  delay(10);
}


void readpins0to2andprint()
{
  // read analog input, divide by 4 to make the range 0-255:
  val1 = (analogRead(0));
  val2 = (analogRead(1));
  val3 = (analogRead(2));

  Serial.print(val1, DEC);
  Serial.print(" ");
  Serial.print(val2, DEC);
  Serial.print(" ");
  Serial.print(val3, DEC);
  Serial.println();
  Serial.print("\r");
  // pause for 10 milliseconds:
  delay(10);  
}


