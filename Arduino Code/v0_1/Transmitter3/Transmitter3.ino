#include "MPU6050Reg.h"

// Declaring an union for the registers and the axis values.
// The byte order does not match the byte order of
// the compiler and AVR chip.
// The AVR chip (on the Arduino board) has the Low Byte
// at the lower address.
// But the MPU-6050 has a different order: High Byte at
// lower address, so that has to be corrected.
// The register part "reg" is only used internally,
// and are swapped in code.
typedef union accel_t_gyro_union
{
  struct
  {
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  } reg;
  struct
  {
    int x_accel;
    int y_accel;
    int z_accel;
    int temperature;
    int x_gyro;
    int y_gyro;
    int z_gyro;
  } value;
};

// Use the following global variables and access functions to help store the overall
// rotation angle of the sensor
unsigned long last_read_time;
float         last_x_angle;  // These are the filtered angles
float         last_y_angle;
float         last_z_angle;
float         last_gyro_x_angle;  // Store the gyro angles to compare drift
float         last_gyro_y_angle;
float         last_gyro_z_angle;

void set_last_read_angle_data(unsigned long time, float x, float y, float z, float x_gyro, float y_gyro, float z_gyro) {
  last_read_time = time;
  last_x_angle = x;
  last_y_angle = y;
  last_z_angle = z;
  last_gyro_x_angle = x_gyro;
  last_gyro_y_angle = y_gyro;
  last_gyro_z_angle = z_gyro;
}

inline unsigned long get_last_time() {return last_read_time;}
inline float get_last_x_angle() {return last_x_angle;}
inline float get_last_y_angle() {return last_y_angle;}
inline float get_last_z_angle() {return last_z_angle;}
inline float get_last_gyro_x_angle() {return last_gyro_x_angle;}
inline float get_last_gyro_y_angle() {return last_gyro_y_angle;}
inline float get_last_gyro_z_angle() {return last_gyro_z_angle;}

//  Use the following global variables and access functions
//  to calibrate the acceleration sensor
float    base_x_accel;
float    base_y_accel;
float    base_z_accel;

float    base_x_gyro;
float    base_y_gyro;
float    base_z_gyro;


int read_gyro_accel_vals(uint8_t* accel_t_gyro_ptr) {
  // Read the raw values.
  // Read 14 bytes at once,
  // containing acceleration, temperature and gyro.
  // With the default settings of the MPU-6050,
  // there is no filter enabled, and the values
  // are not very stable.  Returns the error value

  accel_t_gyro_union* accel_t_gyro = (accel_t_gyro_union *) accel_t_gyro_ptr;

  int error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) accel_t_gyro, sizeof(*accel_t_gyro));

  // Swap all high and low bytes.
  // After this, the registers values are swapped,
  // so the structure name like x_accel_l does no
  // longer contain the lower byte.
  uint8_t swap;
  #define SWAP(x,y) swap = x; x = y; y = swap

  SWAP ((*accel_t_gyro).reg.x_accel_h, (*accel_t_gyro).reg.x_accel_l);
  SWAP ((*accel_t_gyro).reg.y_accel_h, (*accel_t_gyro).reg.y_accel_l);
  SWAP ((*accel_t_gyro).reg.z_accel_h, (*accel_t_gyro).reg.z_accel_l);
  SWAP ((*accel_t_gyro).reg.t_h, (*accel_t_gyro).reg.t_l);
  SWAP ((*accel_t_gyro).reg.x_gyro_h, (*accel_t_gyro).reg.x_gyro_l);
  SWAP ((*accel_t_gyro).reg.y_gyro_h, (*accel_t_gyro).reg.y_gyro_l);
  SWAP ((*accel_t_gyro).reg.z_gyro_h, (*accel_t_gyro).reg.z_gyro_l);

  return error;
}

// The sensor should be motionless on a horizontal surface
//  while calibration is happening
void calibrate_sensors() {
  int                   num_readings = 10;
  float                 x_accel = 0;
  float                 y_accel = 0;
  float                 z_accel = 0;
  float                 x_gyro = 0;
  float                 y_gyro = 0;
  float                 z_gyro = 0;
  accel_t_gyro_union    accel_t_gyro;

  //Serial.println("Starting Calibration");

  // Discard the first set of values read from the IMU
  read_gyro_accel_vals((uint8_t *) &accel_t_gyro);

  // Read and average the raw values from the IMU
  for (int i = 0; i < num_readings; i++) 
  {
    read_gyro_accel_vals((uint8_t *) &accel_t_gyro);
    x_accel += accel_t_gyro.value.x_accel;
    y_accel += accel_t_gyro.value.y_accel;
    z_accel += accel_t_gyro.value.z_accel;
    x_gyro += accel_t_gyro.value.x_gyro;
    y_gyro += accel_t_gyro.value.y_gyro;
    z_gyro += accel_t_gyro.value.z_gyro;
    delay(100);
  }
  
  x_accel /= num_readings;
  y_accel /= num_readings;
  z_accel /= num_readings;
  x_gyro /= num_readings;
  y_gyro /= num_readings;
  z_gyro /= num_readings;

  // Store the raw calibration values globally
  base_x_accel = x_accel;
  base_y_accel = y_accel;
  base_z_accel = z_accel;
  base_x_gyro = x_gyro;
  base_y_gyro = y_gyro;
  base_z_gyro = z_gyro;

  //Serial.println("Finishing Calibration");
}

// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus.
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read.
// There is no function for a single byte.
//
int MPU6050_read(int start, uint8_t *buffer, int size)
{
  int i, n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
    return (-11);

  return (0);  // return : no error
}


// --------------------------------------------------------
// MPU6050_write
//
// This is a common function to write multiple bytes to an I2C device.
//
// If only a single register is written,
// use the function MPU_6050_write_reg().
//
// Parameters:
//   start : Start address, use a define for the register
//   pData : A pointer to the data to write.
//   size  : The number of bytes to write.
//
// If only a single register is written, a pointer
// to the data has to be used, and the size is
// a single byte:
//   int data = 0;        // the data to write
//   MPU6050_write (MPU6050_PWR_MGMT_1, &c, 1);
//
int MPU6050_write(int start, const uint8_t *pData, int size)
{
  int n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);

  return (0);         // return : no error
}

// --------------------------------------------------------
// MPU6050_write_reg
//
// An extra function to write a single register.
// It is just a wrapper around the MPU_6050_write()
// function, and it is only a convenient function
// to make it easier to write a single register.
//
int MPU6050_write_reg(int reg, uint8_t data)
{
  int error;

  error = MPU6050_write(reg, &data, 1);

  return (error);
}

//#include <Wire.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,GyX,GyY,GyZ;

RF24 radio(9, 10);
RF24Network network(radio);
/*struct payload_t {
  unsigned long gyrY;
  unsigned long gurP;
  unsigned long gyrR;
  unsigned long accX;
  unsigned long accY;
  unsigned long accZ;
};*/
struct payload_t{
  int16_t gyrY;
  int16_t gyrP;
  int16_t gyrR;
  int16_t accX;
  int16_t accY;
  int16_t accZ;
};

payload_t payload;

long test = 0;

void setup(){
  //int error;
  uint8_t c;

  //error = MPU6050_read (MPU6050_WHO_AM_I, &c, 1);
  //error = MPU6050_read (MPU6050_PWR_MGMT_2, &c, 1);

  // Clear the 'sleep' bit to start the sensor.
  MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);

  //Initialize the angles
  calibrate_sensors();
  set_last_read_angle_data(millis(), 0, 0, 0, 0, 0, 0);

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  //Serial.begin(9600);
  Serial.begin(9600);

  SPI.begin();
  radio.begin();
  network.begin(90, 01); //this node
}

float FS_SEL;
float gyro_x, gyro_y, gyro_z;
unsigned long t_now;

float alpha = 0.96;
float angle_x;
float angle_y;
float angle_z;  //Accelerometer doesn't give z-angle

float accel_x;
float accel_y;
float accel_z;

float RADIANS_TO_DEGREES = 180/3.14159;
//  float accel_vector_length = sqrt(pow(accel_x,2) + pow(accel_y,2) + pow(accel_z,2));
float accel_angle_y;
float accel_angle_x;
float accel_angle_z;

float dt;
float gyro_angle_x;
float gyro_angle_y;
float gyro_angle_z;

float unfiltered_gyro_angle_x;
float unfiltered_gyro_angle_y;
float unfiltered_gyro_angle_z;

void loop(){
  network.update();

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers


  int error;
  double dT;
  accel_t_gyro_union accel_t_gyro;

  // Read the raw values.
  error = read_gyro_accel_vals((uint8_t*) &accel_t_gyro);
  //doReadSwap();

  // Get the time of reading for rotation computations
  t_now = millis();
  FS_SEL = 131;
  
  gyro_x = (accel_t_gyro.value.x_gyro - base_x_gyro)/FS_SEL;
  gyro_y = (accel_t_gyro.value.y_gyro - base_y_gyro)/FS_SEL;
  gyro_z = (accel_t_gyro.value.z_gyro - base_z_gyro)/FS_SEL;
  

  // Get raw acceleration values
  //float G_CONVERT = 16384;
  accel_x = accel_t_gyro.value.x_accel;
  accel_y = accel_t_gyro.value.y_accel;
  accel_z = accel_t_gyro.value.z_accel;

  // Get angle values from accelerometer
  // float RADIANS_TO_DEGREES = 180/3.14159;
  // float accel_vector_length = sqrt(pow(accel_x,2) + pow(accel_y,2) + pow(accel_z,2));
  accel_angle_y = atan(-1*accel_x/sqrt(pow(accel_y,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
  accel_angle_x = atan(accel_y/sqrt(pow(accel_x,2) + pow(accel_z,2)))*RADIANS_TO_DEGREES;
  accel_angle_z = atan(accel_y/sqrt(pow(accel_x,2) + pow(accel_y,2)))*RADIANS_TO_DEGREES;

  // Compute the (filtered) gyro angles
  dt =(t_now - get_last_time())/1000.0;
  gyro_angle_x = gyro_x*dt + get_last_x_angle();
  gyro_angle_y = gyro_y*dt + get_last_y_angle();
  gyro_angle_z = gyro_z*dt + get_last_z_angle();

  // Compute the drifting gyro angles
  unfiltered_gyro_angle_x = gyro_x*dt + get_last_gyro_x_angle();
  unfiltered_gyro_angle_y = gyro_y*dt + get_last_gyro_y_angle();
  unfiltered_gyro_angle_z = gyro_z*dt + get_last_gyro_z_angle();

  // Apply the complementary filter to figure out the change in angle - choice of alpha is
  // estimated now.  Alpha depends on the sampling rate...
  //float alpha = 0.96;
  angle_x = alpha*gyro_angle_x + (1.0 - alpha)*accel_angle_x;
  angle_y = alpha*gyro_angle_y + (1.0 - alpha)*accel_angle_y;
  angle_z = gyro_angle_z;  //Accelerometer doesn't give z-angle

  // Update the saved data with the latest values
  set_last_read_angle_data(t_now, angle_x, angle_y, angle_z, unfiltered_gyro_angle_x, unfiltered_gyro_angle_y, unfiltered_gyro_angle_z);

  //printData();
  //swapBits();

  AcX = accel_angle_x;
  AcY = accel_angle_y;
  AcZ = accel_angle_z;
  GyX = angle_x;
  GyY = angle_y;
  GyZ = angle_z;

  //payload_t payload = {GyX, GyY, GyZ, AcX, AcY, AcZ};
  payload_t payload = {GyX, GyY, GyZ, AcX, AcY, AcZ};
  RF24NetworkHeader header(00);
  bool ok = network.write(header,&payload,sizeof(payload));
  delay(10);
  //printData();
  //delay(1); /**/
  
}

//Functions used for the Transmitter1 version
void doReadSwap()
{
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

void printData()
{
  // Send the data to the serial port

  Serial.print(F("DEL:"));              //Delta T
  Serial.print(dt, DEC);
  Serial.print(F("#ACC:"));              //Accelerometer angle
  Serial.print(accel_angle_x, 2);
  Serial.print(F(","));
  Serial.print(accel_angle_y, 2);
  Serial.print(F(","));
  Serial.print(accel_angle_z, 2);
  Serial.print(F("#GYR:"));
  Serial.print(unfiltered_gyro_angle_x, 2);        //Gyroscope angle
  Serial.print(F(","));
  Serial.print(unfiltered_gyro_angle_y, 2);
  Serial.print(F(","));
  Serial.print(unfiltered_gyro_angle_z, 2);
  Serial.print(F("#FIL:"));             //Filtered angle
  Serial.print(angle_x, 2);
  Serial.print(F(","));
  Serial.print(angle_y, 2);
  Serial.print(F(","));
  Serial.print(angle_z, 2);
  Serial.println(F(""));

  // Delay so we don't swamp the serial port
  delay(5);
}
