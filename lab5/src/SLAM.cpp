/*********************************************************************************************************
**********************************************************************************************************
							   SLAM.cpp
**********************************************************************************************************
**********************************************************************************************************
	@author		Dylan DeSantis
	@date 		2/26/2018
	@version	1.0.0
**********************************************************************************************************/

#include "SLAM.h"
#include<math.h>

/****************************************************************************************************
	Function Name:  transform
	Inputs: 
		1. int dist = lidar raw measurement distance in cm
		2. int theta1 = servo1 motor angle in degrees 
		3. int theta2 = servo2 motor angle in degrees
	Outputs:
		1. float[] XYZ =  the 3 member float array that we will apply our transformation 
			equations to where X_0 = XYZ[0],  Y_0 = XYZ[1],  Z_0 = XYZ[2]
	Description:
		This function will apply three equations that will transform a local lidar measurement
		taken the end coordinate frame's x-axis into the global xyz coordinates.
 ***************************************************************************************************/
 int SLAM::transform(int dist, int theta1, int theta2, float XYZ[])
 {
 
 		/******
	
	 	COMPLETE  THE TRANSFORMATION EQUATIONS
	
		****/
 	return 0;
 }

SLAM::SLAM()
{
}


SLAM::~SLAM()
{
}
