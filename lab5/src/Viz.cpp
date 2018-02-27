/*********************************************************************************************************
**********************************************************************************************************
							   Viz.cpp
**********************************************************************************************************
**********************************************************************************************************
	@author		Dylan DeSantis
	@date 		2/26/2018
	@version	1.0.0
**********************************************************************************************************/

#include "Viz.h"



/****************************************************************************************************
	Function Name:  initiateLogger
	Inputs: 
	
	Outputs:

	Description:
		This function initiates a filestream to write your measurements to
 ***************************************************************************************************/
int Viz::initiateLogger()
{
	//Function to open the filename
	pFile = fopen(file_name.c_str(),"w");
}

/****************************************************************************************************
	Function Name:  logData
	Inputs: 
		1. float xyz [] = the 3 memeber array of global xyz coordinates from the lidar 
	Outputs:

	Description:
		This writes data to the log file taken from the lidar after being transformed to the 
		global coordinates
 ***************************************************************************************************/
int Viz::logData(float xyz[])
{
	// Function for printing xyz data in a format that vtk can read
	fprintf(pFile,"%f %f %f ", xyz[0],xyz[1],xyz[2]);
}

/****************************************************************************************************
	Function Name:  closLogger
	Inputs: 
	
	Outputs:

	Description:
		This function closes the logger and must be called before the displayPointCloud 
		Function
 ***************************************************************************************************/
int Viz::closeLogger()
{
	// Function for closing logger
	fclose(pFile);
}

/****************************************************************************************************
	Function Name:  displayPointCloud
	Inputs: 
	
	Outputs:

	Description:
		this function displays the main graphical tool used to display point clouds of 
		logged data for post process visualization.
 ***************************************************************************************************/
int Viz::displayPointCloud()
{
	// Function for displaying the point cloud using vtk
	vtkSmartPointer<vtkSimplePointsReader> reader =
  	vtkSmartPointer<vtkSimplePointsReader>::New();
  	reader->SetFileName ( file_name.c_str());
  	reader->Update();
  	vtkSmartPointer<vtkPolyDataMapper> mapper =
    	vtkSmartPointer<vtkPolyDataMapper>::New();
  	mapper->SetInputConnection(reader->GetOutputPort());
  	vtkSmartPointer<vtkAxesActor> axes =
   	vtkSmartPointer<vtkAxesActor>::New();
   	axes->SetTotalLength( 20, 20, 20);

  	vtkSmartPointer<vtkActor> actor =
    	vtkSmartPointer<vtkActor>::New();
  	actor->SetMapper(mapper);
	actor->GetProperty()->SetPointSize(4);
  // Create a renderer, render window, and interactor
  	vtkSmartPointer<vtkRenderer> renderer =
    	vtkSmartPointer<vtkRenderer>::New();
  	vtkSmartPointer<vtkRenderWindow> renderWindow =
    	vtkSmartPointer<vtkRenderWindow>::New();
  	renderWindow->AddRenderer(renderer);
  	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    	vtkSmartPointer<vtkRenderWindowInteractor>::New();
  	renderWindowInteractor->SetRenderWindow(renderWindow);

  	// Add the actor to the scene
  	renderer->AddActor(actor);
   	renderer->AddActor(axes);
  	renderer->SetBackground(.0, .0, .0); // Background color green

  	// Render and interact
  	renderWindow->Render();
  	renderWindowInteractor->Start();
}

