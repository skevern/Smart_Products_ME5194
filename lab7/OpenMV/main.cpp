/****************************************************************
Simple program to communicate with the openMV cam using the 
USB  port. Use the  USB-TLL cables for the Pi and connect the 
green (tx) and white(rx) leads to the RX(P0) and and TX(P1) pins
on the OpenMV camera, respectively. 

To compile:
sudo g++ -o main main.cpp command.cpp command.h Message.cpp Message.h 
Packet.cpp Packet.h Protocol.cpp Protocol.h ProtocolDef.h ProtocolID.h 
RingBuffer.cpp RingBuffer.h Robot.cpp Robot.h Matrix.cpp Matrix.h -lwiringPi
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
#include "Matrix.h"

//Set Serial TX&RX Buffer Size
#define SERIAL_TX_BUFFER_SIZE 64
#define SERIAL_RX_BUFFER_SIZE 256
//Set up file path for Dobot communication
extern int fd;
int fd = serialOpen( UART_PATH, BAUD_RATE);

int camera_to_dobot(int x, int y){
	//Store the four corners as seen by the camera
	float top_left_x = 0;
	float top_right_x = 10;
	float bottom_left_x = 2;
	float bottom_right_x = 8;
	float top_y = 10;
	float bottom_y = 0;
	int width = 10;
	int height = 10;
	
	//Create a matrix the proper size
	Matrix<float> X(width, height);
	Matrix<float> Y(width, height);
	
	//Fill the matrix with the proper X values
	for (int i = 0; i < height; i++){
		float left_ref = (top_left_x + ((bottom_left_x - top_left_x) / (height - 1)) * i);
		float right_ref = (top_right_x - ((top_right_x - bottom_right_x) / (height - 1)) * i);
		
		for (int j = 0; j < width; j++){
			X(i,j) = left_ref + ((right_ref - left_ref)/ (width - 1)) * j;
			//cout << "X: " << X(i,j) << endl;
		}
	}
	//Fill the matrix with the proper Y values
	for (int i = 0; i < height; i++){
		
		for (int j = 0; j < width; j++){
			Y(i,j) = top_y - ((top_y - bottom_y) / (height - 1)) * i;
			//cout << " Y: " << Y(i,j) << endl;
		}
	}
	
	//Determine which of the existing points our dobot point is closest to
	float y_error;
	float x_error;
	float temp_error;
	float total_error = 1000;
	int pos_x;
	int pos_y;
	
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			x_error = abs(x - X(i,j));
			y_error = abs(y - Y(i,j));
			//cout << "x_error is: " << x_error << endl;
			//cout << "y_error is: " << y_error << endl;
			temp_error = pow(pow(x_error,2) + pow(y_error,2),0.5);
			if ( temp_error < total_error){
				total_error = temp_error;
				pos_x = j;
				pos_y = i;
				//cout << "I set a new total error" << endl;
			}
		}
	}
	//cout << "The x position is: " << pos_x << endl;
	//cout << "The y position is: " << pos_y << endl; 
	return pos_x, pos_y;
}



int main (void)
{
	//Testing the camera_to_dobot function
	camera_to_dobot(5,5);
	//Dobot Setup
	int fd = serialOpen( UART_PATH, BAUD_RATE);
	//Robot Dobot;
	//Dobot.setRxInterrupt(100);
	//Dobot.initRAM();
	//ProtocolInit();
	//Dobot.setInitParams();
	
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
      			usleep(1000000);
      		}
      		data = serialGetchar(fd1);
      		printf("Data Recieved: %d \n", data);
      		i++;
      	}
      serialClose(fd1);
  return 0 ;
}

