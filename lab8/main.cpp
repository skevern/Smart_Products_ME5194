/*********************************************************************************************************
**********************************************************************************************************
						main.cpp
**********************************************************************************************************
**********************************************************************************************************
	_filename: main.cpp
	_description: This application uses the NeverRest Am-2964 DC Motor with a quadrature encoder.
	_compiling : To compile this application
		sudo g++ -o main main.cpp MotorPlate.h MotorPlate.cpp SPI_Slave.h SPI_Slave.cpp GPIO.h GPIO.cpp DCMotor.h DCMotor.cpp
	_what_to_complete: will you complete the function in the DCMotor.cpp file:
		1. reference(float time)
		2. readSpeed()
		3. controlSpeed()
				
***************************************References******************************************************
	

************************************************************************************************************
	@author		Dylan DeSantis
	@date 		3/26/2018
	@version	1.0.0
****************************

#include "MotorPlate.h"
#include "SPI_Slave.h"
#include "GPIO.h"
#include "DCMotor.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>


int main()
{
	int spi1_fd = open(SPI_DEV, O_RDWR );
	float sample_time = 0.01;
	std::string fn = "Signal_History.txt";
	DCMotor motor(spi1_fd, fn);
	motor.setupDCMotor(A0, DC_1, CCW_DC, 0, 0, 26, 13);
	motor.setupController(100,1,1,0.01);
	motor.startDCMotor();
	motor.sampleHold(1, S);
	for(int i = 0; i<10000;i++)
	{
		motor.readSpeed();
		motor.controlSpeed();
		motor.sampleHold(10, mS);
	}
	motor.stopDCMotor();
	motor.closeLogger();
	return 0;
}
