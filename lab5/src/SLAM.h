#ifndef SLAM_H
#define SLAM_H
/*********************************************************************************************************
**********************************************************************************************************
							   SLAM.h
**********************************************************************************************************
**********************************************************************************************************
	_filename: SLAM.h
	_description: This is the header file for our models. It will contain the physical models of our
			system, any sensor models, fusion algorithms, and mapping or localization methods
		1. Included in a SLAM model should be a,
			- Physical Model (spatial and kinematics)
			- Control Model
			- Sensor Models (Camera, Lidar, Sonar, etc)
			- Inertial Model (IMU ...to be added)
		2. For this version it will just contain the transmation matrix for the lidar.
				
***************************************References******************************************************
	https://www.mrpt.org/List_of_SLAM_algorithms
	http://zums.ac.ir/files/research/site/ebooks/Computer%20Graphics/Sensor_Fusion_and_its_Applications.pdf

************************************************************************************************************
	@author		Dylan DeSantis
	@date 		2/26/2018
	@version	1.0.0
************************************************************************************************************/


#include "Viz.h"
#include "LidarLite.h"
#include "PanTilt.h"
#include "Camera.h"
#include <math.h>

//	Defining Constants
#define PI  3.14159

class SLAM
{
public:
	/****************************************************************************************************
	 								Memebers 
	 ***************************************************************************************************/
	 
	 
	/****************************************************************************************************
	 								Methods 
	 ***************************************************************************************************/	 
	 int transform(int dist, int theta1, int theta2,  float* XYZ);

public:
	/****************************************************************************************************
	 								Constructor/ Destructor 
	 ***************************************************************************************************/
	SLAM();
	~SLAM();
};
#endif

