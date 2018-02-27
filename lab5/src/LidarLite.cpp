/*********************************************************************************************************
**********************************************************************************************************
							   PanTilt.cpp
**********************************************************************************************************
**********************************************************************************************************
	@author		Dylan DeSantis
	@date 		2/26/2018
	@version	1.0.0
**********************************************************************************************************/


#include "LidarLite.h"
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
	Function Name:  read8
	Inputs: 
		1. int reg = register to read
	Outputs:
		2. int result = the byte that is returned
	Description:
		reads a byte from a register after taking control of the i2c bus
 ***************************************************************************************************/

int LidarLite::read8(int reg)
{
	this->takeI2CBus();
	wiringPiI2CWrite(this->fd_i2c, reg);
	int result =wiringPiI2CRead(this->fd_i2c);
	usleep(1000);
	return result;
}

/****************************************************************************************************
	Function Name:  write8
	Inputs: 
		1. int reg = register to write to.
		2. int data = the byte to write 
	Outputs:

	Description:
		This writes a byte to the desired register after taking control of the i2c bus
 ***************************************************************************************************/
int LidarLite::write8(int reg, int data)
{
	this->takeI2CBus();
	wiringPiI2CWrite(this->fd_i2c, reg);
	wiringPiI2CWrite(this->fd_i2c, data);
	wiringPiI2CWriteReg8(this->fd_i2c, reg, data);
	usleep(1000);
	return 0;
}

/****************************************************************************************************
	Function Name:  read16
	Inputs: 
		1. int reg = the register to start reading 16-bits from
	Outputs:
		1. int result = the combined 2 byte word 
	Description:
		this function reads 2 bytes from  a register and returns there convoluted result
 ***************************************************************************************************/
int LidarLite::read16(int reg)
{
	this->takeI2CBus();
	wiringPiI2CWrite(this->fd_i2c, 0x0F);
	int result1 =wiringPiI2CRead(this->fd_i2c);
	wiringPiI2CWrite(this->fd_i2c, 0x10);
	int result2 = wiringPiI2CRead(this->fd_i2c);
	int result = (result1 << 8) | result2;
	return result;
}

/****************************************************************************************************
	Function Name:  write16
	Inputs: 
		1. int reg = the register to write to
		2. int data = the 2 bytes of data to write 
	Outputs:

	Description:
		this function  writes to bytes of data to a register
 ***************************************************************************************************/
int LidarLite::write16(int reg, int data)
{
	this->takeI2CBus();
	wiringPiI2CWriteReg16(this->fd_i2c, reg, data);
	return 0;
}

/****************************************************************************************************
	Function Name:  connect
	Inputs: 
		
	Outputs:
		
	Description:
		this function initializes and connects to the Lidar
 ***************************************************************************************************/
int LidarLite::connect()
{
	configure(0);
	return 0;
}

/****************************************************************************************************
	Function Name:  configure
	Inputs: 
		1. int config = the configuration to set the type of performance we want
	Outputs:

	Description:
		This function set the configuration of the Lidar's measurement system to output
		a given accuracy and time response.
 ***************************************************************************************************/
int LidarLite::configure(int config)
{

	switch (config)
	{
	case 0: // Default mode, balanced performance
		this->write8(SIG_COUNT_VAL, 0x80); // Default
		this->write8(ACQ_CONFIG_REG, 0x08); // Default
		this->write8(THRESHOLD_BYPASS, 0x00); // Default
		break;

	case 1: // Short range, high speed
		this->write8(SIG_COUNT_VAL, 0x1D);
		this->write8(ACQ_CONFIG_REG, 0x08); // Default
		this->write8(THRESHOLD_BYPASS, 0x00); // Default
		break;

	case 2: // Default range, higher speed short range
		this->write8(SIG_COUNT_VAL, 0x80); // Default
		this->write8(ACQ_CONFIG_REG, 0x00);
		this->write8(THRESHOLD_BYPASS, 0x00); // Default
		break;

	case 3: // Maximum range
		this->write8(SIG_COUNT_VAL, 0xff);
		this->write8(ACQ_CONFIG_REG, 0x08); // Default
		this->write8(THRESHOLD_BYPASS, 0x00); // Default
		break;

	case 4: // High sensitivity detection, high erroneous measurements
		this->write8(SIG_COUNT_VAL, 0x80); // Default
		this->write8(ACQ_CONFIG_REG, 0x08); // Default
		this->write8(THRESHOLD_BYPASS, 0x80);
		break;

	case 5: // Low sensitivity detection, low erroneous measurements
		this->write8(SIG_COUNT_VAL, 0x80); // Default
		this->write8(ACQ_CONFIG_REG, 0x08); // Default
		this->write8(THRESHOLD_BYPASS, 0xb0);
		break;
	}
	return 0;
}

/****************************************************************************************************
	Function Name:  distance
	Inputs: 
		1. bool bias_correct = whether or not to return a bias correct measurement
	Outputs:
		1. int dist = the distance in centimeters of the lidar measurement
	Description:
		This function reads the distance measurements from the lidar lite  module and 
		checks to make sure that the hardware is in working condition. It waits until 
		the lidar has finished with its current measurements before grabbing the data.
 ***************************************************************************************************/
int LidarLite::distance(bool bias_correct)
{
	

	/**** 
			Initiate the aquistion with or without the 
			bias correction included.
	****/
	if (bias_correct)
	{
		this->write8(ACQ_COMMAND, 0x04);
	}
	else
	{
		this->write8(ACQ_COMMAND, 0x03);
	}
	int i = 0;
	do{
		if (i == 100000) return 0;
		int stat = this->status();
		if ( (stat & BUSY_FLAG) == 0)
		{
			return this->read16(READ_DIST);
		}
		else if ((stat & BUSY_FLAG) == BUSY_FLAG)
		{
			//printf("still busy \n");
			i++;
			usleep(100);
			continue;
		}
		else
		{
			switch (stat)
			{
			case PROCESS_ERROR:
				printf("Process Error when collecting data\n");
				return -1;
			case HEATH_STATUS:
				printf("Health Status Error when collecting data\n");
				return -1;
			case SECONDARY_RET:
				printf("Secondary Return Error when collecting data\n");
				return -1;
			case INVALID_SIG:
				printf("Invaild Signal Error when collecting data\n");
				return -1;
			case SIG_OVERFLOW:
				printf("Signal Overflow Error when collecting data\n");
				return -1;
			case REF_OVERFLOW:
				printf("Reference Overflow Error when collecting data\n");
				return -1;
			default:
				printf("Error when collecting data, status = %X \n", stat);
				return -1;
			}
		}
	} while (1);
	return 0;
}


/****************************************************************************************************
	Function Name:  reset
	Inputs: 

	Outputs:

	Description:
		resets the Lidar Lite module to its default settings, you must wait 22 milliseconds
		for it to reboot.
 ***************************************************************************************************/
int LidarLite::reset()
{
	this->write8(ACQ_COMMAND, 0x00);
	usleep(22000);
	return 0;
}

/****************************************************************************************************
	Function Name:  sigStrength
	Inputs: 
		
	Outputs:
		1. int ss = returns the signal strength of the measurement
	Description:
		This is to be implemented in the future, and would return the signal strength of the 
		last measurement.
 ***************************************************************************************************/
int LidarLite::sigStrength()
{
	return 0;
}

/****************************************************************************************************
	Function Name:  status
	Inputs: 
		
	Outputs:
		1. int FLAG or zero		
	Description:
		 checks the status of the LidarLite and returns the error flag if incountered otherwise
		 returns 0;
 ***************************************************************************************************/
int LidarLite::status()
{
	/*
	Checks the status of the LidarLite
	*/
	int stat = this->read8(STATUS);
	if ((stat & PROCESS_ERROR) == PROCESS_ERROR)	return PROCESS_ERROR;
	if ((stat & HEATH_STATUS) != HEATH_STATUS)		return HEATH_STATUS;
	if ((stat & SECONDARY_RET) == SECONDARY_RET)	return SECONDARY_RET;
	if ((stat & INVALID_SIG) == INVALID_SIG)		return INVALID_SIG;
	if ((stat & SIG_OVERFLOW) == SIG_OVERFLOW)		return SIG_OVERFLOW;
	if ((stat & REF_OVERFLOW) == REF_OVERFLOW)		return REF_OVERFLOW;
	if ((stat & BUSY_FLAG) == BUSY_FLAG)			return BUSY_FLAG;
	return 0;
}

/****************************************************************************************************
	Function Name:  takeI2CBus
	Inputs: 
		
	Outputs:
	
	Description:
		 This function takes control of the I2C bus
 ***************************************************************************************************/
int LidarLite::takeI2CBus()
{
	/*
	Sets the I2C to communicate with this device
	*/
	if (ioctl(this->fd_i2c, I2C_SLAVE, this->lidar_address) < 0)
	{
		printf("problem setting slave\n");
		return -1;
	}
	return 0;
}
