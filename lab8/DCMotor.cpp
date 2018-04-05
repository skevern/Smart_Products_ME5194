/*********************************************************************************************************
**********************************************************************************************************
							 DCMotor.cpp
**********************************************************************************************************
**********************************************************************************************************
	@author		Dylan DeSantis
	@date 		3/25/2018
	@version	1.0.0
**********************************************************************************************************/
#include "GPIO.h"
#include "SPI_Slave.h"
#include "DCMotor.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <chrono>
#include <ctime>

int DCMotor::setupDCMotor(PLATE_ADDR addr, DC_MOTOR mtr_in, DC_MOTOR_DIR mtr_dir, float start_speed, float accel, int A_Encoder_Pin, int B_Encoder_Pin)
{
	this->pa = addr;
	this->mtr = mtr_in;
	this->dir = mtr_dir;
	this->pin_enA = A_Encoder_Pin;
	this->pin_enB = B_Encoder_Pin;
	MotorPlate::setup();
	MotorPlate::configDC( addr, mtr_in,  mtr_dir, start_speed, accel);
	MotorPlate::GPIO::pinMode(A_Encoder_Pin, INPUT);
	MotorPlate::GPIO::pinMode(B_Encoder_Pin, INPUT);
	pFile = fopen(file_name.c_str(),"w");
	fprintf(pFile,"Time (s), Ref (deg/s), Ctrl (deg/s), Error (deg/s), Output (deg/s) \n");
	return 0;
}
int DCMotor::setupController(float Kp, float Ki, float Kd, float Ts)
{
	this->K1 = Kp + Ki*Ts/2.0 + Kd/Ts;
	this->K2 = Ki*Ts/2.0-Kp-2.0*Kd/Ts;
	this->K3 = Kd/Ts;
	this->Ts = Ts;
	return 0;
}
int DCMotor::closeLogger()
{
	fclose(pFile);
	return 0;
}

float DCMotor::time()
{
	std::chrono::steady_clock::time_point time_now = std::chrono::steady_clock::now();
	std::chrono::duration<float, std::micro> time_span = time_now-this->start_time ;
	float duration =(1.0/1000000.0)* ((float)(time_span.count()));
	return duration;
}

int DCMotor::sampleHold(int delay, timeScale ts)
{
	MotorPlate::delay(delay, ts);
	return 0;
}

int DCMotor::startDCMotor()
{
	MotorPlate::startDC(this->pa, this->mtr);
	this->start_time = std::chrono::steady_clock::now();
	return 0;
}

int DCMotor::stopDCMotor()
{
	MotorPlate::stopDC(this->pa, this->mtr);
	this->end_time = std::chrono::steady_clock::now();
	return 0;
}

int DCMotor::updateSpeed(float speed)
{
	float duty_cycle = (speed/((float)MAX_DPS))*100.0;
	MotorPlate::putDCSpeed(this->pa, this->mtr, duty_cycle);
	return 0;
}

int DCMotor::logSignals(float time, float ref, float ctrl, float err, float actual)
{
	fprintf(pFile,"%f, %f, %f, %f, %f \n",time, ref, ctrl, err, actual);
	return 0;
}
int DCMotor::update_error_hist(float err)
{
	this->error_sig_km2 = this->error_sig_km1;// error for the k-2 sample
	this->error_sig_km1 =this->error_sig_km0 ;// error for the k-1 sample
	this->error_sig_km0 = err; // error for the k-0 sample
	return 0;
}

int DCMotor::update_control_hist(float ctrl)
{
	this->ctrl_sig_km1 = this->ctrl_sig_km0;// control for the k-1 sample
	this->ctrl_sig_km0 = ctrl;// control for the k-0 sample
	return 0;
}

float DCMotor::saturation(float speed)
{
	if(speed > ((float)MAX_DPS)) {speed = ((float)MAX_DPS-10);}
	if(speed<0) {speed =0;}
	return speed;
}

float DCMotor::reference(float time)
{
	//ramp up
	while (time <= 10)
	{
		speed = (960/10)*time;
	}
	
	//plateau
	while ( time > 10 && time < 20)
	{
		speed=960; 
	}
	
	//ramp down
	while ( time > 20 && time <= 30)
	{
		speed= speed - (960/10);
	}
	
	return speed;
}



float DCMotor::readSpeed()
{
	int readValA= MotorPlate::GPIO::digitalRead(this->pin_enA );         //reads the current state of encoders and stores its value
	int readValB= MotorPlate::GPIO::digitalRead(this->pin_enB );  
	float timeout = DCMotor::time();
	float to_val = 0.1;
	while ((readValA == MotorPlate::GPIO::digitalRead(this->pin_enA )) && (readValB == MotorPlate::GPIO::digitalRead(this->pin_enB )))  //when we exit the while loop we have switched states 
	{
		if((DCMotor::time()-timeout)> to_val){return 0.0;}
	}
	float t1= time(); //starts the timer
	readValA= MotorPlate::GPIO::digitalRead(this->pin_enA );         //reads the current state of encoders and stores its value
	readValB= MotorPlate::GPIO::digitalRead(this->pin_enB );  
	to_val = 0.1;
	while ((readValA == MotorPlate::GPIO::digitalRead(this->pin_enA )) && (readValB == MotorPlate::GPIO::digitalRead(this->pin_enB )))  //when we exit the while loop we have switched states 
	{

	}
	float t2=time(); //stops the timer 
	float elapsed_time= t2-t1;  //time in between states 
	float speed= 1/(elapsed_time*(1120));  //rotational speed in rev/s 
	return speed;
}



int DCMotor::controlSpeed(float reference_speed)
{
	//Fill Code In Here
	float err = readSpeed() - reference_speed;
	update_error_hist(err);												//Log our current error into the history
	float control_value = this->ctrl_sig_km1 + this->K1 * err + this->K2 * this->error_sig_km1 + this->K3 * this->error_sig_km2;
	control_value = saturation(control_value);							//Use saturation to make sure we remain within our limits
	update_control_hist(control_value);									//Update control history
	return int(control_value);
}

