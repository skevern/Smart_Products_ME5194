#ifndef LIDAR_LITE_H
#define	LIDAR_LITE_H
/*********************************************************************************************************
**********************************************************************************************************
							   LidarLite.h
**********************************************************************************************************
**********************************************************************************************************
	_filename: LidarLite.h
	_description: This class controls the communication protocols with the LIDARLite v3 module
			produced by Garmin. This class uses I2C to communicate with the RPi and 
			includes the basic functionality to measure distances with capabilities to 
			implement more customized settings.
				
***************************************References******************************************************
https://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf
https://github.com/garmin/LIDARLite_v3_Arduino_Library/blob/master/src/LIDARLite.cpp
************************************************************************************************************
	@author		Dylan DeSantis
	@date 		2/26/2018
	@version	1.0.0
************************************************************************************************************/


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
#include <deque>
#include <wiringPiI2C.h>


//	LidarLite I2C Address
#define LL_I2C_ADDR		0x62
#define LL_I2C_W_ADDR	0xC4
#define	LL_I2C_R_ADDR	0xC5


//	Defining LidarLite register Addresses

#define ACQ_COMMAND			0x00	//	W	Receiver Bias Correction
#define	STATUS				0x01	//	R	System Status
#define SIG_COUNT_VAL		0x02	//	RW	Max Acquistions per returned signal (0x80)
#define	ACQ_CONFIG_REG		0x04	//	RW	Acq. Mode (0x08)
#define	VELOCITY			0x09	//	R	Velocity measurement output
#define	PEAK_CORR			0x0C	//	R	Peak value in correlation record
#define	NOISE_PEAK			0x0D	//	R	Correlation record noise floor
#define	SIGNAL_STRENGTH		0x0E	//	R	Recieved signal strength
#define	FULL_DELAY_HIGH		0x0F	//	R	Distance measurement high byte
#define	FULL_DELAY_LOW		0x10	//	R	Distance measurement low byte
#define	READ_DIST			0x8F	//	R	Reads two bytes from the LIDAR
#define	OUTER_LOOP_COUNT	0x11	//	RW	Burst measurement count control (0x01)
#define	REF_COUNT_VAL		0x12	//	RW	Reference acq. count (0x05)
#define	LAST_DELAY_HIGH		0x14	//	R	Prev. distance measur. high byte
#define	LAST_DELAY_LOW		0x15	//	R	Prev. distance measur. low byte
#define	UNIT_ID_HIGH		0x16	//	R	Serial number high byte
#define UNIT_ID_LOW			0x17	//	R	Serial number low byte
#define	I2C_ID_HIGH			0x18	//	W	Write serial number high byte for address unlock
#define	I2C_ID_LOW			0x19	//	W	Write serial number low byte for address unlock
#define	I2C_SEC_ADDR		0x1A	//	RW	Write new i2c address after unlock
#define	THRESHOLD_BYPASS	0x1C	//	RW	Peak detection threshold bypass	(0x00)
#define	I2C_CONFIG			0x1E	//	RW	Default address response control
#define	COMMAND				0x40	//	RW	State command
#define	MEASURE_DELAY		0x45	//	RW	Delay between automatic measur. (0x14)
#define	PEAK_BCK			0x4C	//	R	Second largest peak value in correlation record
#define	CORR_DATA			0x52	//	R	Correlation record data low byte
#define	CORR_DATA_SIGN		0x53	//	R	Correlation record data high byte
#define	ACQ_SETTINGS		0x5D	//	RW	Correlation record memory bank select
#define	POWER_CONTROL		0x65	//	RW	Power state control (0x80)

//	Status Error Flags
#define PROCESS_ERROR		0x40	// Process error = 1
#define HEATH_STATUS		0x20	// Health is ok = 1
#define SECONDARY_RET		0x10	// Secondary returned = 1
#define INVALID_SIG			0x08	// Invalid signal returned = 1, if peak detected = 0
#define SIG_OVERFLOW		0x04	// Signal Overflow = 1
#define REF_OVERFLOW		0x02	// Reference data in correlation record reached max value
#define BUSY_FLAG			0x01	// Device is busy taking measurement



class LidarLite
{
public:

	/****************************************************************************************************
	 								Memebers 
	 ***************************************************************************************************/
	uint8_t lidar_address = LL_I2C_ADDR;
	int		fd_i2c;

public:
	/****************************************************************************************************
	 								Methods 
	 ***************************************************************************************************/
	int read8(int reg);
	int read16(int reg);
	int takeI2CBus();
	int write8(int reg, int data);
	int write16(int reg, int data);
	

	int connect();
	int configure(int config);
	int distance(bool bias_correct);
	int isBusy();
	int sigStrength();
	int status();
	int reset();


	/****************************************************************************************************
	 								Constructors/Destructors 
	 ***************************************************************************************************/

	LidarLite(int fd):
		fd_i2c(fd){}
	~LidarLite() {}

};

#endif
