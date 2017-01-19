/*
 Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
 Update 2014 - TMRh20
 */

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(9,10);                // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 00;    // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 02;   // Address of the other node in Octal format

/*struct payload_t {                 // Structure of our payload
  unsigned long y;
  unsigned long p;
  unsigned long r;
  unsigned long accY;
  unsigned long accX;
  unsigned long accZ;
};*/

struct payload_t {
  float y;
  float p;
  float r;  
  float accY;
  float accX;
  float accZ;
};

int counter = 0;

RF24NetworkHeader header;        // If so, grab it and print it out
payload_t payload;

void setup(void)
{
  Serial.begin(57600);
  Serial.println("RF24Network/examples/helloworld_rx/");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop(void)
{  
  doAction();
  //doTest();
}

void doAction()
{
  network.update();                  // Check the network regularly
  while ( network.available() ) 
  {     // Is there anything ready for us?
    network.read(header,&payload,sizeof(payload));
    doSerialPrint();
  }
}

void doTest()
{
  header.from_node = 0;
  payload.y = counter;
  payload.p = counter;
  payload.r = counter;
  payload.accX = counter;
  payload.accY = counter;
  payload.accZ = counter;
  counter++;
  if(counter>300)
    counter = 0;
  doSerialPrint(); 
  delay(200);
}

void doSerialPrint()
{
    Serial.print(header.from_node);
    Serial.print('\t');
    Serial.print((int) payload.y);
    Serial.print('\t');
    Serial.print((int) payload.p);
    Serial.print('\t');
    Serial.print((int) payload.r);
    Serial.print('\t');
    Serial.print((int) payload.accY);
    Serial.print('\t');
    Serial.print((int) payload.accX);
    Serial.print('\t');
    Serial.println((int) payload.accZ);
    delay(10);
}
