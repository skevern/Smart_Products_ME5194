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
				
***************************************References********************************************************
	

************************************************************************************************************
	@author		Dylan DeSantis
	@date 		3/26/2018
	@version	1.0.0
************************************************************************************************************/

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
	motor.setupController(1.432, 1849000, 0, 0.00000444);
	motor.startDCMotor();
	
	/*motor.updateSpeed(500);
	motor.sampleHold(5,S);
	float sum = 0;
	for (int i = 0; i < 10; i++){
		cout << "Motor Speed is: " << motor.readSpeed() << endl;
		motor.sampleHold(1, S);
		sum += motor.readSpeed();
	}
	cout << "The average motor speed was: " << sum/10 << endl; */
	
	for(float i = 0.0; i < 20; i += 0.2)
	{
		float ref_speed = motor.reference(i);
		float new_speed = motor.controlSpeed(ref_speed);
		motor.updateSpeed(new_speed);
		motor.sampleHold(200, mS);
		cout << "\nDesired (Reference) Speed is: " << ref_speed << endl;
		cout << "Actual (Encoder) Speed is: " << motor.readSpeed() << endl;
		cout << "New Control Speed is: " << new_speed << endl;
	}
	motor.updateSpeed(0);
	
	motor.stopDCMotor();
	motor.closeLogger();
	return 0;
}
