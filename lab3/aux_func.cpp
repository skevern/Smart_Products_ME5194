//====================================================================
// ------------------------aux_func.cpp -----------------------
//====================================================================
/**
	_filename: aux_func.cpp
	_description: This file performs the auxillary function for transfer data from a main
		application. The current configuration has the input of:
			argv[1] = "xNUM(1)xNUM(2)x...xNUM(n)x
		where n is the number of packets of information that has been declared in the
		main file as the variable BUFFER_SZ. To decode the incoming message we 
		use a simple string splitting mechanism with "x" being a generic deliminator.
		Use this template to decode information about your main process, and send
		it to the bus arbiter  (FPGA) using an SPI protocol. 
		
		compile
		===========================================
		g++ -o aux_func aux_func.cpp lab2_gpio.o SPI_Slave.o
======================================================================
	@author Dylan DeSantis	Github: DDDeSantis
	@version 1.0.0
	@date 1/18/2018
*/
#include "lab2_gpio.h"
#include "SPI_Slave.h"
#include <stdio.h>
#include<stdlib.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include<string.h>
#include<deque>
//	Define Encoding Scheme
#define RED		0xA0
#define BLUE	0xB0
#define GREEN	0xC0
#define SIZE_SM	0x01
#define SIZE_MD	0x02
#define SIZE_LG	0x03
// Masks
#define COLOR_MASK 0xF0
#define SIZE_MASK 	0x0F
//	Path to the debug file
//	=============CHANGE THESE PATH FILES ============
#define PATH_2_LOG "/home/pi/Desktop/Smart Products/lab3/aux_debug.txt"
//	==================================================
std::string getColor( unsigned long packet)
{
	unsigned long color = packet & COLOR_MASK;
	if(color == RED) 		return "RED";
	if(color == GREEN)	return "GREEN";
	if(color == BLUE)		return "BLUE";
	else					return "N/A";
}

std::string getSize( unsigned long packet)
{
	unsigned long size = packet & SIZE_MASK;
	if(size == SIZE_SM) 		return "SMALL";
	if(size == SIZE_MD)		return "MEDIUM";
	if(size == SIZE_LG)		return "LARGE";
	else						return "N/A";
}

 using namespace std;
int main(int argc, char *argv[])
{
//========================================================
// --------------FIRST DEBUGGING---------------------
//========================================================
	//	For debugging purposes (write to the log )
	
	static FILE *pFile = fopen(PATH_2_LOG,"w");
	//	Pringt out all of the incoming information
	for (int i = 0; i<argc; i++)
	{
		fprintf(pFile,"argv[%d] = %s \n", i, (argv[i]));
	}
//========================================================
// ------------------DECODING MESSAGE-----------------
//========================================================
	std::deque<unsigned char>  encrypted_bytes;// used to hold the tokens
	char *dup = strdup(argv[1]);//	copy the string
	char *token = strtok(dup, "x"); //split string
	while(token !=NULL)
	{
		unsigned long val = (unsigned)std::atol(token);
		encrypted_bytes.push_back((unsigned char)val);
		token = strtok(NULL, "x");
	}
//========================================================
// ---------------------PURGE RESULTS---------------
//========================================================
//	Example how to release information from a deque
//	getColor and getSize  decodes the message back into human
//	language
	for( int i = 0 ; i<encrypted_bytes.size(); i++)
	{
		fprintf(pFile," Byte number (%d) : Value = %X , Color = %s, Size = %s \n",
				i, 
				(int)encrypted_bytes[i],
				 getColor((unsigned long)encrypted_bytes[i]).c_str(), 
				 getSize(encrypted_bytes[i]).c_str());
	}	
	fclose(pFile);
//========================================================
// ------------------SEND INFORMATION--------------------
//========================================================
// 	Use this area to send informtion to FPGA through SPI

//========================================================
// -------------GPIO MASTER BUS RELEASE--------------
//========================================================
//	Use this area to signal the FPGA that you are finished controlling
//	The SPI bus line
//	This example simply switches a line to an led 
	lab2_gpio Sess_aux; 
	int pin_num = 2; // pin to control
	Sess_aux.pinMode(pin_num, OUTPUT);// set the pinmode for output 
	int pin_val = Sess_aux.digitalRead(pin_num); // read the value of the pin 
	if (pin_val ==LOW) pin_val = HIGH;
	else pin_val = LOW;
	Sess_aux.digitalWrite(pin_num, (outValue)pin_val); // change LED
	Sess_aux.~lab2_gpio();// invoke the destructor and unmap the address	 

	return 0;
}
