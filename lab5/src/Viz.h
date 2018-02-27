#ifndef VIZ_H
#define VIZ_H
/*********************************************************************************************************
**********************************************************************************************************
							   Viz.h
**********************************************************************************************************
**********************************************************************************************************
	_filename: Viz.h
	_description: This class handles the data logging of the sensors and displays the point cloud
			of a data set. 
			
			As of the current version this class only provides the means to visualize logged data
			via post processing. Future versions intend to implement a real-time version
				
***************************************References******************************************************
	https://lorensen.github.io/VTKExamples/site/Cxx/IO/SimplePointsReader/
	https://lorensen.github.io/VTKExamples/site/Python/GeometricObjects/Axes/
	https://www.kitware.com/products/books/VTKUsersGuide.pdf

************************************************************************************************************
	@author		Dylan DeSantis
	@date 		2/26/2018
	@version	1.0.0
************************************************************************************************************/

#include<sstream>
#include<fstream>
#include<string.h>
#include<cstring>

#include <vtkSimplePointsReader.h>
#include <vtkPointSource.h>
#include<vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAxes.h>
#include <vtkAxesActor.h>
#include <vtkTubeFilter.h>


class Viz
{
public:
	/****************************************************************************************************
	 								Memebers 
	 ***************************************************************************************************/
	std::string file_name;
	FILE *pFile;
	
		 
	/****************************************************************************************************
	 								Methods 
	 ***************************************************************************************************/	 
	int initiateLogger();
	int logData(float *xyz);
	int closeLogger();
	int displayPointCloud();
	
	
	/****************************************************************************************************
	 								Constructor/ Destructor 
	 ***************************************************************************************************/
	Viz(std::string fn) :
		file_name(fn) 
		{}
	~Viz() {}
};
#endif
