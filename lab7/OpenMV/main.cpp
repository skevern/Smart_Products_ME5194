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
#include <thread>
#include <chrono>        

// Set Serial TX&RX Buffer Size
#define SERIAL_TX_BUFFER_SIZE 64
#define SERIAL_RX_BUFFER_SIZE 256
// Set up file path for Dobot communication
extern int fd;
int fd = serialOpen( UART_PATH, BAUD_RATE);

int Search(){
	//Open the USB port and set up wiringPi
	int fd1 = serialOpen ("/dev/ttyUSB0", 9600);
	wiringPiSetup (); 
	fflush(stdout);
	
	//Send requests until the camera responds to us
	bool request_received = false;
	int data = 0;
	int num = 0;
	int input = 0;
	
	while(request_received == false){
		
		cout << "Input the number to send to the camera: ";
		cin >> input;	
		serialPutchar (fd1, int(input));
		num = 0;
		while (serialDataAvail(fd1) == 0){
			num = serialDataAvail(fd1);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		data = serialGetchar(fd1);
		cout << "The returned value was: " << data << endl;
		request_received = true;
		}
    serialClose(fd1);
	return data;
}

int main (void)
{
	
	//Dobot Setup
	int fd = serialOpen( UART_PATH, BAUD_RATE);
	Robot Dobot;
	Dobot.setRxInterrupt(100);
	Dobot.initRAM();
	ProtocolInit();
	Dobot.setInitParams();
	int top_left[] = {280,145};
	int top_right[] = {300,-60};
	int bottom_left[] = {105,135};
	int bottom_right[] = {120,-75};
	int z_scan = 25;
	
	//Scanning routine
	//Dobot.goToXYZ(top_left[0], top_left[1], z_scan, true);				//Move to the top left
	int return_from_search = Search();									//Ask the camera for ball positions
	cout << "Return from search: " << return_from_search << endl; 		
	//Testing the dobot to move to coordinates we specify
	int x;
	int y;
	int z;
	int start = 0;
	while(start == 0){
		/*cout << "Enter the x point for the dobot:" << endl;
		cin >> x;
		cout << "Enter the y point for the dobot:" << endl;
		cin >> y;
		cout << "Enter the z point for the dobot:" << endl;
		cin >> z;*/
		Dobot.goToXYZ(x, y, z, true);
		/*Dobot.goToXYZ(225, -75, 130, true);
		Dobot.goToXYZ(120,-75,0,true);
		Dobot.goToXYZ(105,135,0,true);
		Dobot.goToXYZ(280,145,0,true);
		Dobot.goToXYZ(300,-60,0,true);
		Dobot.goToXYZ(225,-60,130,true);
		Dobot.goToXYZ(150,-195,130,true);
		Dobot.goToXYZ(225, -75, 130, true);*/
	}
  return 0 ;
}

