//==============================================================================
// ---------------------------------------------------------- lab2_post_Q1.cpp -----------------------------------------------------------------
//==============================================================================
/**
	_filename: lab2_post_Q1.cpp
	_description: This main program employs the Digilent Pmod IA which is an impedance analyzer
	built around the Analog Devices AD5933 12-bit Impedance Converter. The communication to 
	the Pmod IA is carried out over the I2C bus on the raspberry pi. The Pmod IA is initated as a 
	I2C_Slave object from the imported header file "I2C_Slave.h" which utilizes the Linux based 
	I/O control functions, specifically the  " ioctl(...) "  function. The I2C configurations used is 
	the I2C1 bus.On the raspberry pi 3B this corresponds with the pin configuration :
			-------------------------------------------------
			| 	SDA  ---> Pin 3			|
			| 	SCL  ---> Pin 5			|
			-------------------------------------------------
	The program shows the simple frequency sweep with incremental  frequency steps. The 
	programable settings allow for the start frequency, frequency resolution, settling time, the
	number of increments, the peak to peak voltage, and the gain to be selected for the specific
	application. The program also makes use of different read/write routines avaliable for the
	Pmod IA over I2C including single byte read/write, sequential read/write commands, and 
	block read/write commands. The measurements need to be calibrated for each Pmod IA
	calculating for the gain factor. One can find the instructions for this calibration and other 
	specific command values in the references below.
	
	-----------------------------------------------------References----------------------------------------------------------------------
	https://reference.digilentinc.com/reference/pmod/pmodia/reference-manual
	https://reference.digilentinc.com/_media/reference/pmod/pmodia/pmodia_sch.pdf
	http://www.analog.com/media/en/technical-documentation/data-sheets/AD5933.pdf
===============================================================================
	@author Dylan DeSantis
	@version 1.0.0
*/
#include "I2C_Slave.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include<math.h>

using namespace std;

#define	PI	3.14159265

int main()
{

	int i2c1_fd = open(I2C1_PATH, O_RDWR );
	unsigned char result;
	unsigned char slaveAddress =0x0D; // slave address
	unsigned char slaveRegister = 0x80; // control reg
	unsigned char IA_MASK = 0xF;
	unsigned char STAND_BY = 0xB;
	unsigned char INITIALIZE = 0x1;
	unsigned char START = 0x2;
	unsigned char FreqIn = 0x3;
	unsigned char Vpp = 0x0<<1;
	unsigned char Gain = 0x0;
	unsigned char Settle1 = 0x0;
	unsigned char Settle2 = 0x4;
	unsigned char rgstr, ptr_cmd, blk_cmd;
	
//=====================================================================
// ------------------------------------------------ Initializing I2C Device ----------------------------------------------------
//=====================================================================
	I2C_Slave PmodIA(slaveAddress, i2c1_fd);
	PmodIA.i2cBegin();
	PmodIA.i2cWrite8( 0x81, 0x10);
//=====================================================================
// --------------------------------- Putting device into stand-by mode ----------------------------------------------
//=====================================================================	
	result =  PmodIA.i2cRead8(slaveRegister); // current mode of PmodIA
	unsigned char masked_rgstrs = STAND_BY << 4;
	masked_rgstrs = (masked_rgstrs |Vpp ) | Gain;
	unsigned char  bitmask =  ~(IA_MASK <<4);// create a mask to have all 0's in the place of pin pinNum slots
	unsigned char  filtSelect = result & bitmask;// take the current config. and zero out the slot
	unsigned char newSelect = filtSelect | masked_rgstrs;	
	PmodIA.i2cWrite8( 0x80, masked_rgstrs);

//=====================================================================
// -----------------------Setting up frequency sweep measurement parameters -----------------------
//=====================================================================
//xxxx The three routines below are identical in their end results, but provide different
//xxxx methods of carrying out the routine. Uncomment just one routine at a time.
//~~~~~~~~~~~ Regular write routine ~~~~~~~~~~~~~~
/*
	// Start Frequency
	PmodIA.i2cWrite8(  0x82, 0x0F); 
	PmodIA.i2cWrite8( 0x83, 0x5C); 
	PmodIA.i2cWrite8( 0x84, 0x28); 
	//frequency resolution	
	PmodIA.i2cWrite8(  0x85, 0x00); 
	PmodIA.i2cWrite8( 0x86, 0x01); 
	PmodIA.i2cWrite8( 0x87, 0x4F); 
	// number of increments
	PmodIA.i2cWrite8(  0x88, 0x00); 
	PmodIA.i2cWrite8( 0x89, 0xE1); 
 	//cycle times for settling
	//PmodIA.i2cWrite8( 0x8A, Settle1); 
	PmodIA.i2cWrite8( 0x8B, Settle2); 
	cout << "I performed the Regular Write Routine" << endl; 
*/
//~~~~~~~~~~~~ Sequential write routine ~~~~~~~~~~~~~~~
/*
	unsigned char settings_rgstrs[] = {0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8B};
	unsigned char settings_cmds[] = {0x0F, 0x5C, 0x28, 0x00, 0x01, 0x4F, 0x00, 0xE1, 0x28};
	PmodIA.i2cPutBlkCmds(settings_rgstrs, settings_cmds, 9, 0);
	PmodIA.i2cStreamOutSeq();
	cout << "I performed the Sequential Write Routine" << endl;
*/
//~~~~~~~~~~~~ Block write routine ~~~~~~~~~~~~~~~~~~~

	//PmodIA.i2cWrite8( 0x8A, Settle1); 
	PmodIA.i2cWrite8( 0x8B, Settle2); 
	unsigned char settings_rgstrs[] = {0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89};
	unsigned char settings_cmds[] = {0x0F, 0x5C, 0x28, 0x00, 0x01, 0x4F, 0x00, 0xE1};
	PmodIA.i2cPutBlkCmds(settings_rgstrs, settings_cmds, 8, 0);
	rgstr  = 0x82; 		// register to start the block write
	ptr_cmd = 0xB0;		
	blk_cmd = 0xA0;
	PmodIA.i2cStreamOutBlock(rgstr, ptr_cmd, blk_cmd);
	cout << "I performed the Block Write Routine" << endl;
//=====================================================================
// -------------------------------- Put into frequency sweep initialize mode -------------------------------------
//=====================================================================	
	result =  PmodIA.i2cRead8(slaveRegister); 
	masked_rgstrs = INITIALIZE << 4;// Initialize mask 
	 bitmask =  ~(IA_MASK <<4);
	filtSelect = result & bitmask;
	newSelect = filtSelect | masked_rgstrs;
	PmodIA.i2cWrite8( 0x80, newSelect); 
	cout << "I put us into frequency sweep initialize mode" << endl;
//=====================================================================
// ------------------------------------------ Starting Frequency Sweep --------------------------------------------------
//=====================================================================	
	result = PmodIA.i2cRead8(slaveRegister); 
	masked_rgstrs = START << 4;// start mask
	 bitmask =  ~(IA_MASK <<4);
	filtSelect = result & bitmask;
	newSelect = filtSelect | masked_rgstrs;
	PmodIA.i2cWrite8( 0x80, newSelect); 
	cout << "I started the frequency sweep" << endl;
//=====================================================================
// -------------------------------------------- Frequency Increment Loop ----------------------------------------------
//=====================================================================
	unsigned char  ContMask = (1<<2); //mask to determine if we should  continue to increment
	unsigned char freqstop= 0; //stop incrementing frequency condition
	int i = 0;
	//Block Read Settings
	rgstr  = 0x94; // register to start the block read
	ptr_cmd = 0xB0;
	blk_cmd = 0xA1;
	unsigned char num_blk_reads = 0x4;
	
	unsigned char status_read;
	PmodIA.i2cClearQueues();
	while((int)freqstop ==0)
	{
		unsigned char DataMask = (1 << 1);// mask to determine if data is ready to be sampled
		unsigned char dataAval = 0;
		while((int)dataAval == 0)
		{
			unsigned char status_read =   PmodIA.i2cRead8( 0x8F); 
			dataAval = status_read & DataMask;
			//printf(" %X \n", (int)dataAval);
		}
		// Block Read 
	/*	cout << "Before StreamInBlock" << endl; 
		PmodIA.i2cStreamInBlock(num_blk_reads, rgstr, ptr_cmd, blk_cmd);
		cout << "After StreamInBlock" << endl; 
		double real_data = (double) (((PmodIA.stream_in[0]) << 8) | (PmodIA.stream_in[1]));
		double imag_data =(double) (((PmodIA.stream_in[2]) << 8) | (PmodIA.stream_in[3]));
		PmodIA.stream_in.clear();
		*/
		//Single Byte Read

		unsigned char real_MSB=  PmodIA.i2cRead8( 0x94); 
		unsigned char real_LSB =   PmodIA.i2cRead8( 0x95); 
		unsigned char imag_MSB =   PmodIA.i2cRead8( 0x96); 
		unsigned char imag_LSB =   PmodIA.i2cRead8( 0x97); 	
		double real_data = (double) ((real_MSB << 8) | real_LSB);
		double imag_data = (double) ((imag_MSB << 8) | imag_LSB);

		
		double mag_resp =sqrt( pow(real_data, 2) + pow(imag_data, 2)); //Magnitude response
		double phase_resp  = atan2(imag_data, real_data) *180/PI; //phase response
		
		double GF1 = 1/(680*1000*65535.0); // Gain factor calibration
		//double GF2 = 1/(10*1000*65525.0);
		double impedance = 1/(GF1*mag_resp);
		printf("Real  Data: measurement %d  is %f \n", i,real_data);
		printf("Imag Data: measurement %d  is %f \n", i,imag_data);
		printf("Increment %d: Mag = %f, Phase = %f  \n", i, mag_resp, phase_resp);
		// increment one step
		result =  PmodIA.i2cRead8( 0x80);
		status_read =  PmodIA.i2cRead8( 0x8F); 
		freqstop = status_read & ContMask;
		masked_rgstrs = FreqIn << 4;
		bitmask =  ~(IA_MASK <<4);
		filtSelect = result & bitmask;
		newSelect = filtSelect | masked_rgstrs;
		PmodIA.i2cWrite8( 0x80, newSelect); 
		 i = i+1; 
	}

	close(i2c1_fd);
	//PmodIA.~I2C_Slave();// invoke the destructor and unmap the address
	return 0;
}

