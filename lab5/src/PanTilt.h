#ifndef PAN_TILT_H
#define	PAN_TILT_H
/*********************************************************************************************************
**********************************************************************************************************
							   PanTilt.h
**********************************************************************************************************
**********************************************************************************************************
	_filename: SLAM.h
	_description: This class controls the servo motors of the PanTilt board that is produced by
			pimoroni. It communicates via i2c and can drive the two motors.
				
***************************************References******************************************************
https://github.com/pimoroni/pantilt-hat/blob/master/library/pantilthat/pantilt.py#L507
https://pinout.xyz/pinout/pan_tilt_hat

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

//	PanTilt I2C address
#define	PT_I2C_ADDR			0x15

//	PanTilt Registers
#define	PT_CONFIG			0x00	//	
#define	PT_SERVO_1		0x01	//		Panning motor
#define	PT_SERVO_2		0x03	//		Tilting motor
#define	PT_LIGHT			0x05	//
#define	PT_UPDATE			0x4E	//		

//	Constants
#define MAX_US				2325
#define	MIN_US				575
#define I2C1_PATH "/dev/i2c-1"	

typedef enum {
	SERVO_1 = 0,
	SERVO_2 = 1
}SERVO_NUM;

class PanTilt
{

public:
	/****************************************************************************************************
	 								Memebers 
	 ***************************************************************************************************/

	/*	Configuration Byte */
	uint8_t enable_sv1	= 0x01;
	uint8_t	enable_sv2	= 0x01;
	uint8_t	enable_light	= 0x00;
	uint8_t	light_mode	= 0x01;
	uint8_t	light_on		= 0x00;

	bool	is_setup;		
	int		fd_i2c;
	float	update_wait	= 0.3;
	int		sv1_max;
	int		sv1_min;
	int		sv2_max;
	int		sv2_min;
	int		sv_min_max[2][2];
	int		sv1_timeout;
	int		sv2_timeout;


public:
	/****************************************************************************************************
	 								Methods 
	 ***************************************************************************************************/
	void		configure();
	void		setup();
	int			deg_2_us(int angle);
	int			us_2_deg(int micro_sec);
	int			get_angle(SERVO_NUM servo_num);
	int			set_angle(SERVO_NUM servo_num, int angle);
	//int			check_range(int value, int value_min, int value_max);
	int			enable_servo(SERVO_NUM servo_num);
	int			disable_servo(SERVO_NUM servo_num);
	void		at_exit();



	/****************************************************************************************************
	 								Constructors/ Destructors 
	 ***************************************************************************************************/
	PanTilt(int fd) :
		fd_i2c(fd)
	{
		this->sv1_max = MAX_US;
		this->sv1_min = MIN_US;
		this->sv2_max = MAX_US;
		this->sv2_min = MIN_US;
		this->sv_min_max[0][0] = this->sv1_min;
		this->sv_min_max[0][1] = this->sv1_max;
		this->sv_min_max[1][0] = this->sv2_min;
		this->sv_min_max[1][1] = this->sv2_max;
		this->sv1_timeout = 0;
		this->sv2_timeout = 0;
		this->is_setup = false;
				
	}

	~PanTilt();
};

#endif 
