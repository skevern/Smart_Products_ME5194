//====================================================================
// ----------------------- setup_program.cpp ----------------------
//====================================================================
/**
	_filename: setup_program.cpp
	_description: This file will register a new character device and create a special file
	which allows you to acces it via the open command.
======================================================================
	@author Dylan DeSantis
	@version 1.0.0
	@date 1/18/2018
*/

#include <stdio.h>
#include<stdlib.h>
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
#define MODULE_PATH 	"/home/pi/Documents/smart_products/lab3/lab3_make/rev1/interrupt_module_mmap.ko"
#define AUX_EXEC_PATH 	"/home/pi/Documents/smart_products/lab3/lab3_make/rev1/test/aux_func"
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


int getMajor(std::string dev_name)
{
	std::stringstream express, conv;
	express<<"grep "<<dev_name<<" /proc/devices > logger.txt";
	int str = system(express.str().c_str());
	std::string line;
	std::ifstream logger("logger.txt");
	std::getline(logger,line);
	logger.close();
	char *dup = strdup(line.c_str());
	char *token = strtok(dup, " ");
	int val = std::atoi(token);
	printf("%d \n",val);
	//conv<<"mknod "<<dev_name<<" c "<<val<<" 0";
	//system(conv.str().c_str());
	//dev_t dev_num =  makedev(val, 0);
	//int status1 = mknod(dev_name.c_str(), S_IFCHR, dev_num);
	free(dup);
	return val;
}


void makeNode(int majorNum, std::string dev_name )
{	
	
	/*
	std::stringstream express;
	express<<"mknod node S_IFCHR "<<majorNum<<" 0";
	int str = system(express.str().c_str());
	*/
	//printf("hh%d \n", majorNum);
	dev_t dev_num =  makedev(majorNum, 0);
	int status1 = mknod(dev_name.c_str(), S_IFCHR, dev_num);
	return;
}


int main()
{
//========================================================
// -------------INITIAL SETUP-----------------
//========================================================
	//	Setup the buffers and  page settings
	int NPAGES = 2;// should correspond to the kernel module number
	int BUFFER_SZ = 16; //  32  byte buffer size
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
	int major = getMajor("lab3_dev");
	system(remove_stream.str().c_str());
	makeNode(major,"lab3_dev");
}
	
