/*********************************************************************************************************
**********************************************************************************************************
						lab5_main.cpp
**********************************************************************************************************
**********************************************************************************************************
	_filename: lab5_main.cpp
	_description: This is the main application for lab5, in which you will analyze and derive the 
			kinematics and transformations of the servo system as well as  complete the 
			data acquison of your envirornment through Lidar. This is the first step in 
			the process of Simultaneous localization and mapping (SLAM). This application 
			has several dependencies, so to be to make sure that you have all the correct
			packages installed. 
					1. build-essential linux-libc-dev
					2. cmake cmake-gui 
					3. libusb-1.0-0-dev libusb-dev libudev-dev 
					4. libeigen3-dev pkg-config
					5. qt5-default
					6. qtcreator
					7. libvtk6.3
					8. vtk6
					9. python-vtk6
					10. libvtk6-*
					11. libvtk6.3-qt
					12. libflann1.9
					13. libflann-dev libboost1.62-all-dev
	_compiling : To compile this application we will cmake / make.  The folder layout is 
			- lab5
				-- src
					-- sources and CMakeLists.txt files
				-- build
					-- where we will build the files
			Once you finish your main application in this file, naviagate to the build folder and 
			perform the following commands.
				1. sudo cmake ../src
				2. sudo make
			Once you have completed these steps you can then run your application
				1. sudo ./Slam
				
***************************************References******************************************************
	
	https://larrylisky.com/2016/11/03/point-cloud-library-on-ubuntu-16-04-lts/
	https://hackster.imgix.net/uploads/attachments/271378/pi-pinout.png

************************************************************************************************************
	@author		Dylan DeSantis
	@date 		2/26/2018
	@version	1.0.0
************************************************************************************************************/


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>        
#include<errno.h>
#include<math.h>
#include<signal.h>
#include<unistd.h>
#include "PanTilt.h"
#include "LidarLite.h"
#include "Viz.h"
#include "SLAM.h"
#include "Camera.h"
#include <wiringPiI2C.h>
#include<sstream>
#include<fstream>
#include<string.h>

 using namespace std;

int main()
{

	/****************************************************************************************************
	 								Setup 
	 ***************************************************************************************************/
	std:string fname = "./LidarData.xyz";				// Logger File name
	int i2c1_fd = wiringPiI2CSetup(LL_I2C_ADDR);	// Setting up shared I2C Bus
	/******
	
	 	COMPLETE THE SETUP
	
	****/

	
	/****************************************************************************************************
	 						Main Data Collection Loop:
	 ***************************************************************************************************
	 	1. Use the Viz functions:
	 		a. initiateLogger() - to initiate the begining of the data logger
	 		b. logData( float[3] xyz) - to log an array of the global xyz lidar measurement
	 		c. closeLogger() - to end the data logging proces
	 		d. displayPointCloud() - to display the logged data AFTER YOU HAVE FINISHED GATHERING ALL YOUR DATA
	 	2. Use the Slam functions:
	 		a. transform(int dist, int angle1, int angle2,  float [3] xyz)  - to calculate
	 				the x, y, z coordinates of the lidar measurement data ,dist, at a
	 				given combination of servo angels (angle1, angle2). the result fills
	 				the float xyz array  x = xyz[0], y = xyz[1],  z = xyz [2]
	 	3. Use the PanTilt functions
	 		a. set_angle(SERVO1, int angle1) - to set the angle of servo1 angle, angle must
	 				be between 90 and -90 degrees.
	 		b. set_angle(SERVO2, int angle2) - to set the angle of servo2 angle, angle must
	 				be between 90 and -90 degrees.
	 		c. get_angle(SERVO1) - to return an integer of the current angle of servo1
	 		d. get_angle(SERVO2) - to return an integer of the current angle of servo2
	 	4. Use the LidarLite functions:
	 		a.  distance(true); - to return the integer distance an object in cm
	 	5. Use the function usleep(int microseconds) to create delays between actuation,
	 		feedback measurement and lidar measurements to create a stable scanning motion.
	***********************************************************************************************************/
	
	
	
	
		/******
	
	 	COMPLETE THE MAIN LOOP
	
		****/
	
	
	
	

	
	
	/**************************************************************
		End Data log & Begin Display Point Cloud
	****************************************************************/
	
		/******
	
	 	COMPLETE THE MAIN LOOP
	
		****/
		

  return EXIT_SUCCESS;
}


	
