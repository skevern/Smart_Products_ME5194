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
	this->stateA = MotorPlate::GPIO::digitalRead(A_Encoder_Pin );
	this->stateB = MotorPlate::GPIO::digitalRead(B_Encoder_Pin );
	pFile = fopen(file_name.c_str(),"w");
	fprintf(pFile,"Time (s), Ref (deg/s), Ctrl (deg/s), Error (deg/s), Output (deg/s) \n");
	return 0;
}
int DCMotor::setupController(float Kp, float Ki, float Kd, float Ts)
{
	this->K1= Kp + Ki*Ts/2.0 + Kd/Ts;
	this->K2 = Ki*Ts/2-Kp-2*Kd/Ts;
	this->K3 = Kd/Ts;
	return 0;
}
int DCMotor::closeLogger()
{
	fclose(pFile);
	return 0;
}

float DCMotor::time()
{
	float duration = ( clock() - this->start_time ) / (float) CLOCKS_PER_SEC;
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
	this->start_time = std::clock();
	return 0;
}

int DCMotor::stopDCMotor()
{
	MotorPlate::stopDC(this->pa, this->mtr);
	this->end_time = std::clock();
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
// Fill in Code Here
	return speed;
}

float DCMotor::readSpeed()
{
// Fill In Code HERE


	return speed;
}

int DCMotor::controlSpeed()
{
//Fill Code In Here


	return 0;
}

