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
	Function Name:  tran
	* sform
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
 int SLAM::transform(float dist, float theta1, float theta2, float XYZ[])
 {
	 
	float d1 = 4;																
	float a1 = 3;
	float ay2 = 2;
	float ax2 = 9 + dist;
	float r1 = sqrt(pow(d1 , 2.0) + pow(a1 , 2.0));
	float r2 = sqrt(pow(ay2 , 2.0) + pow(ax2 , 2.0));
	
	float r2_angle = atan(ay2 / ax2);

	float x_0 = 0;
	float y_0 = 0;
	float z_0 = 0;
	
	float x1 = x_0 + a1 * cos(theta1);
	float y1 = y_0 + a1 * sin(theta1);
	float z1 = z_0 + d1;
	
	float x2 = x1 + r2 * cos(theta2 + r2_angle) * cos(theta1);
	float y2 = y1 + r2 * cos(theta2 + r2_angle) * sin(theta1);
	float z2 = z1 + r2 * sin(theta2 + r2_angle);
	
	XYZ[0] = x2;
	XYZ[1] = y2;
	XYZ[2] = z2;
	
	return 0;
 }

SLAM::SLAM()
{
}


SLAM::~SLAM()
{
}
