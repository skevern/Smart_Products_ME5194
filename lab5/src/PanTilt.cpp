/*********************************************************************************************************
**********************************************************************************************************
							   PanTilt.cpp
**********************************************************************************************************
**********************************************************************************************************
	@author		Dylan DeSantis
	@date 		2/26/2018
	@version	1.0.0
**********************************************************************************************************/



#include "PanTilt.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <chrono>
#include <ctime>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <wiringPiI2C.h>
#include<math.h>

/****************************************************************************************************
	Function Name:  Setup
	Inputs: 

	Outputs:

	Description:
		Sets up the motor board.
 ***************************************************************************************************/
void PanTilt::setup()
{
	if (this->is_setup) return;
	this->configure();
	this->is_setup = true;
	return;
}

/****************************************************************************************************
	Function Name:  configure
	Inputs: 

	Outputs:

	Description:
		sets the configuration of the pan tilt board. 
 ***************************************************************************************************/
void PanTilt::configure()
{
	uint8_t	config = 0x00;
	config |= (this->enable_sv1) | (this->enable_sv2 << 1) | (this->enable_light << 2)
		| (this->light_mode << 3) | (this->light_on << 5);

	if (ioctl(this->fd_i2c, I2C_SLAVE, PT_I2C_ADDR) < 0)
	{
		printf("problem setting slave\n");
	}	
	wiringPiI2CWriteReg8(this->fd_i2c,PT_CONFIG,config);
	return;
}

/****************************************************************************************************
	Function Name:  deg_2_us
	Inputs: 
		1. int angle = servo motor angle in degs 
	Outputs:
		1. int us = the microsecond pulse width equivalent
	Description:
		This is a simple PWM pulse width conversion given a desired degree.
 ***************************************************************************************************/
int PanTilt::deg_2_us(int angle)
{
	angle += 90;
	if (angle > 180 || angle < 0)
	{
		printf(" error angle is not between +-90 deg, angle = %d \n", angle - 90);
		return -1;
	}
	float range = (float)(this->sv_min_max[0][1] - this->sv_min_max[0][0]);
	float micro_sec = ((range / 180.0)*((float)angle));
	int u_s = ((int)this->sv_min_max[0][0]) + ((int)micro_sec);
	return (u_s);
}

/****************************************************************************************************
	Function Name:  us_to_deg
	Inputs: 
		1. int us = given a microsecond pulse width
	Outputs:
		1. int angle  = the equivalent angle in deg
	Description:
		This is a simple angle conversion given a desired PWM pulse width
 ***************************************************************************************************/

int PanTilt::us_2_deg(int micro_sec)
{
	int sv_range = this->sv_min_max[0][1] - this->sv_min_max[0][0];
	float angle = (((float)(micro_sec - this->sv_min_max[0][0])) / float(sv_range))*180.0;
	return ((int)(floor(angle))-90);
}

/****************************************************************************************************
	Function Name:  get_angle
	Inputs: 
		1. SERVO_NUM servo_num = either SERVO1 or SERVO2
	Outputs:
		1. int angle =  the current angle of servo_num
	Description:
		This function will get a reading from the pan tilt board for a given servo motor
 ***************************************************************************************************/
int PanTilt::get_angle(SERVO_NUM servo_num)
{
	int us;
	if (ioctl(this->fd_i2c, I2C_SLAVE, PT_I2C_ADDR) < 0)
	{
		printf("problem setting slave\n");
	}
	if (servo_num == SERVO_1)
	{
		us = wiringPiI2CReadReg16(this->fd_i2c,PT_SERVO_1);
	}
	else
	{
		us = wiringPiI2CReadReg16(this->fd_i2c,PT_SERVO_2);
	}

	return this->us_2_deg(us);
}

/****************************************************************************************************
	Function Name:  set_angle 
	Inputs: 
		1. SERVO_NUM servo_num = either SERVO1 or SERVO2 
		2. int angle = the desired angle we want to go to 
	Outputs:

	Description:
		This function sets the specified servo motor to the desired angle in degrees
 ***************************************************************************************************/
int PanTilt::set_angle(SERVO_NUM servo_num, int angle)
{
	this->setup();
	if (servo_num == SERVO_1)
	{
		if (!(this->enable_sv1))
		{
			this->enable_sv1 = true;
			this->configure();
		}
		int us = this->deg_2_us(angle);
		if (ioctl(this->fd_i2c, I2C_SLAVE, PT_I2C_ADDR) < 0)
		{
			printf("problem setting slave\n");
		}
		wiringPiI2CWriteReg16(this->fd_i2c,0x01,us);
	}
	else
	{
		if (!(this->enable_sv2))
		{
			this->enable_sv2 = true;
			this->configure();
		}
		int us = this->deg_2_us(angle);
		if (ioctl(this->fd_i2c, I2C_SLAVE, PT_I2C_ADDR) < 0)
		{
			printf("problem setting slave\n");
		}
		int st = wiringPiI2CWriteReg16(this->fd_i2c,0x03,us);
	}
	return 0;
}

/****************************************************************************************************
	Function Name:  enable_servo 
	Inputs: 
		1. SERVO_NUM servo_num = either SERVO1 or SERVO2 
	Outputs:

	Description:
		This function sets enables (powers on) the specified servo motor
 ***************************************************************************************************/
int PanTilt::enable_servo(SERVO_NUM servo_num)
{
	this->setup();
	if (servo_num == SERVO_1)
	{
		this->enable_sv1 = true;
	}
	else
	{
		this->enable_sv2 = true;
	}
	this->configure();
	return 0;
}

/****************************************************************************************************
	Function Name:  disable_servo 
	Inputs: 
		1. SERVO_NUM servo_num = either SERVO1 or SERVO2 
	Outputs:

	Description:
		This function sets disables (powers off) the specified servo motor
 ***************************************************************************************************/
int PanTilt::disable_servo(SERVO_NUM servo_num)
{
	this->setup();
	if (servo_num == SERVO_1)
	{
		this->enable_sv1 = false;
	}
	else
	{
		this->enable_sv2 = false;
	}
	this->configure();
	return 0;
}


PanTilt::~PanTilt()
{
	disable_servo(SERVO_1);
	disable_servo(SERVO_2);
}
