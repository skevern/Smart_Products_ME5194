/****************************************************************
Simple program to communicate with the openMV cam using the 
USB  port. Use the  USB-TLL cables for the Pi and connect the 
green (tx) and white(rx) leads to the RX(P0) and and TX(P1) pins
on the OpenMV camera, respectively. 

To compile:
sudo g++ -o main main.cpp command.cpp command.h Message.cpp Message.h 
Packet.cpp Packet.h Protocol.cpp Protocol.h ProtocolDef.h ProtocolID.h 
RingBuffer.cpp RingBuffer.h Robot.cpp Robot.h -lwiringPi
******************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>        
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sstream>
#include <fstream>
#include <string.h>
#include "Robot.h"
//Set Serial TX&RX Buffer Size
#define SERIAL_TX_BUFFER_SIZE 64
#define SERIAL_RX_BUFFER_SIZE 256
//Set up file path for Dobot communication
extern int fd;
int fd = serialOpen( UART_PATH, BAUD_RATE);

int main (void)
{
	//Dobot Setup
	int fd = serialOpen( UART_PATH, BAUD_RATE);
	Robot Dobot;
	Dobot.setRxInterrupt(100);
	Dobot.initRAM();
	ProtocolInit();
	Dobot.setInitParams();
	
	int fd1 = serialOpen ("/dev/ttyUSB0", 9600);
	wiringPiSetup (); 
	fflush (stdout);
 	int i = 0;
 	int data = 0;
	while(i<10)
	{
		int send_data = 5;
      		serialPutchar (fd1, send_data);
      		int num = 0;
      		while(num==0)
      		{
      			num = serialDataAvail(fd1);
      			printf("Checking for serial data available\n");
      			usleep(1000000);
      		}
      		data = serialGetchar(fd1);
      		printf("Data Recieved: %d \n", data);
      		i++;
      	}
      serialClose(fd1);
  return 0 ;
}
