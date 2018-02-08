//====================================================================
// ------------------- lab3_main.cpp -------------------
//====================================================================
/**
	_filename: lab3_main.cpp
	_description: This main file is used as the main application in a system
	that controls a continuous process, while being susceptible to interrupts from
	external systems in which it must communicate with or process other services.
	
	The current configuration of this three file compilation simply registers 
	
	compile the file using the lab2_gpio.o file
	==================================================
	g++ -o lab3_main lab3_main.cpp lab2_gpio.o
======================================================================
	@author Dylan DeSantis
	@version 1.0.0
	@date 1/18/2018
*/

#include <stdio.h>
#include<stdlib.h>
#include"lab2_gpio.h"
#include<iostream>
#include<sstream>
#include<string.h>
#include<cstring>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<fstream>
#include<sys/sysmacros.h>
//	Define Encoding Scheme
#define RED		0xA0
#define BLUE	0xB0
#define GREEN	0xC0
#define SIZE_SM	0x01
#define SIZE_MD	0x02
#define SIZE_LG	0x03
//	Paths for module, auxillary function, module input params, and string combinations
//	=============CHANGE THESE PATH FILES ============
#define MODULE_PATH 	"/home/pi/Desktop/Smart Products/lab3/Make_folder/interrupt_module_mmap.ko"
#define AUX_EXEC_PATH 	"/home/pi/Desktop/Smart Products/lab3/aux_func"
//	==================================================
#define EXEC_PATH		"exec_path"
#define ACK_PIN			"ack_pnum"
#define TRIG_PIN 		"trig_pnum"
#define BUFF_SIZE		"buff_size"
#define SP				" "
#define EQ				"="
#define QQ				"\""
#define INS				"/sbin/insmod"
#define RM				"/sbin/rmmod"
 
int main()
{
//========================================================
// -------------------INITIAL SETUP-----------------------
//========================================================
	//	Setup the buffers and  page settings
	int NPAGES = 4;// should correspond to the kernel module number
	int BUFFER_SZ = 32; //  32  byte buffer size
	int PAGE_SIZE = getpagesize();

	//	Declare the pins and while loop iterators
	int pin_out_ack = 17; //	We use this pin to send acknowledgements/ requests to bus arbiter
	int pin_in_trig = 27; // 		This is our pin which we will designate as the interrupt pin
	int pin_set_trig = 22;//		This is for testing purposes, this simulates a trigger
	int break_pin = 3;//		Button pin to break out of the main loop and end the application
	int i = 0;//				While loop itereator
	
	//	Create Sting commands for inserting and removing the module
	std::stringstream insert_stream, remove_stream;
	//	xxxINSERTING
	insert_stream<<INS<<SP<<MODULE_PATH;
	insert_stream<<SP<<ACK_PIN<<EQ<<pin_out_ack;
	insert_stream<<SP<<TRIG_PIN<<EQ<<pin_in_trig;
	insert_stream<<SP<<EXEC_PATH<<EQ<<QQ<<AUX_EXEC_PATH<<QQ;
	insert_stream<<SP<<BUFF_SIZE<<EQ<<BUFFER_SZ;
	
	//	xxxREMOVING
	remove_stream<<RM<<SP<<MODULE_PATH;
	//	Insert Module
	system(insert_stream.str().c_str());
	//	Map the kernel module memory space into the userspace for direct access
	int fd = open("lab3_dev", O_RDWR | O_SYNC);
	if(fd<0) printf("ERROR Opening Device, %d \n",fd);
	//	Specify the size of our memory space
	int len = NPAGES * PAGE_SIZE;
	//	Mapping in Direct memory access to Kernel space
	void *ptrx;
	ptrx = mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptrx==MAP_FAILED) printf("ERROR Mapping Device, %d \n", 0);
	//	Initialize GPIO object and declare PINMODE
	lab2_gpio Sess_main;
	Sess_main.pinMode(break_pin, INPUT); // How we breakout of our main loop
	Sess_main.pinMode(pin_set_trig,OUTPUT); // simulate a trigger signal 
	
//========================================================	

//========================================================
// --------------------MAIN  LOOP-----------------------
//========================================================
	std::cout<<" BEGIN PROGRAM"<<std::endl;
	//The first element keep tracks of the number of current packets
	*(( volatile unsigned int *)ptrx) = 0;	
	int packet_count = 1;
	while(Sess_main.digitalRead(break_pin))
	{
		//Simulate a pulse into the interrupt pin
		if(i==800) {
			std::cout<<"Simulate Trigger Signal: Rising Edge "<<std::endl;
			Sess_main.digitalWrite(pin_set_trig, HIGH);
		}
		else if(i==8000) {
			std::cout<<"Simulate Trigger Signal: Falling Edge "<<std::endl;
			Sess_main.digitalWrite(pin_set_trig, LOW);
		}
		else if(i >=80 && packet_count<33)
		{
			if(packet_count ==1){
			*(( volatile unsigned int *)ptrx+packet_count) = RED | SIZE_SM;	
			printf("Buffer: %X, reading \n",*((unsigned int *)ptrx));
			*(( volatile unsigned int *)ptrx) = 1;	
			packet_count++;
			}
			else{
			*(( volatile unsigned int *)ptrx+packet_count) = 0x00;	
			*(( volatile unsigned int *)ptrx) = packet_count;	
			packet_count++;
			}
			
		}
		else {}
		i++;
	}
	std::cout<<" END PROGRAM"<<std::endl;
	//	Remove Module & destroy GPIO object
	munmap(ptrx, len);
	close(fd);
	system(remove_stream.str().c_str());
	Sess_main.~lab2_gpio();
	
	return 0;	
//========================================================
}
