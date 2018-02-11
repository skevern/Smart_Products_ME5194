/**
_filename: I2C_Slave.cpp
_description: Definition file for a I2C slave class.
//=====================================================================
// ------------------------------------ I2C Slave Class Definitions ------------------------------------------------
// ____________________________________________________________________________
// xxxx This class defines a general layout for slave devices connected to the 
// xxxx master. Use this class a parent class for specific slave devices with which
// xxxx you communicate via I2C
//=====================================================================
@author Dylan DeSantis
@version 1.0.0
*/
#include "I2C_Slave.h"
#include <iostream>

using namespace std;
//=====================================================================
// -------------------------------------- METH: Open/closing and utilities -------------------------------------------
//=====================================================================

void I2C_Slave::i2cBegin()
{
	/**
		Function to set the slave address	
	*/
	ioctl(this->i2c_fd, I2C_SLAVE, this->slv_addr);
}

int I2C_Slave::i2cEnd()
{
	/**
		Function to end any concurent threads (optional)
	*/	
}

void I2C_Slave::i2cDelay(int delay_time)
{
	/**
		General function to provide a delay in the program in microseconds.
		
		@param delay_time the amount of time to delay the program in microseconds	
	*/
 	auto start = std::chrono::system_clock::now();
 	double  time_comp = 0.0;
		while(time_comp <delay_time)
		{
			std::chrono::duration<double> elapsed_time = std::chrono::system_clock::now() - start;
			time_comp =(elapsed_time.count())*1000.0;
		}
}
void I2C_Slave::i2cPutBlkCmds(unsigned char rgstr[], unsigned char cmds[], int numCmds, int rw)
{
	/**
		Function for placing the next communication data into (-->) the deques: cmds --> stream_out, rgstr --> register2read or register2write
		
		@param rgstr an array of size numCmds, with each entry's value being the register to read, or the register to write the corresponding command to.	
		@param cmds an array of size numCmds, with each entry's value being the command  to  write  to the corresponding register in rgstr.	
		@param numCmds an integer value denoting the total number of commands to read or to write
		@param rw an integer value denoting the operation of the commands, rw = 0 (write) or rw = 1 (read)
		
	*/
	if (rw ==0)
	{
		for (int i = 0; i<numCmds; i++)
		{
			this->stream_out.push_back(cmds[i]);
			this->register2write.push_back(rgstr[i]);
		}
	} 
	else{
		for (int i = 0; i<numCmds; i++)
		{
			this->register2read.push_back(rgstr[i]);
		}
	}
}
	
void I2C_Slave::i2cClearQueues()
{
	/**
		Function to clear the deques: stream_in, stream_out, register2read, register2write.	
	*/
	this->register2read.clear();
	this->register2write.clear();
	this->stream_in.clear();
	this->stream_out.clear();
}

//=====================================================================
// -------------------------------------- METH: Read/ Write  ----------------------------------------------------------------
//=====================================================================
unsigned char I2C_Slave::i2cRead8(unsigned char rgstr)
{
		/**
			Function to read an 8-bit word from a register of the slave device 
			
			@param rgstr	an character value of the register from which to read 8-bits
			
			@return data byte that is read from slave
		*/

	//~~~~~~~~~~~~~~~~~~~Enter Code Here ~~~~~~~~~~~~~~~~~~~~~~~~~
	unsigned char inbuf, outbuf;
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];

	//First, we "dummy write" 0 bits to the register we want to read from
	outbuf = rgstr;
	messages[0].addr = this->slv_addr;
	messages[0].flags = 0;
	messages[0].len = sizeof(outbuf);
	messages[0].buf = &outbuf;

	//Data will be returned in this structure
	messages[1].addr = this->slv_addr;
	messages[1].flags = I2C_M_RD;
	messages[1].len = sizeof(inbuf);
	messages[1].buf = &inbuf;

	//Send the request to the kernel and get the result back
	packets.msgs = messages;
	packets.nmsgs = 2;
	if(ioctl(this->i2c_fd, I2C_RDWR, &packets) < 0) 
	{
		perror("Unable to read data, whoops");
		return 1;
	}
	unsigned char data_in = inbuf;
	return data_in;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}

int I2C_Slave::i2cWrite8(unsigned char rgstr, unsigned char data_byte)
{
		/**
			Function to write  an 8-bit word to a register
			
			@param rgstr	the register which the word is written to
			@param data_byte	the byte of data that is to be written to the register
			
			@return status of write process-  0 for no errors and -1 for errors
		*/

	//~~~~~~~~~~~~~~~~~~~Enter Code Here ~~~~~~~~~~~~~~~~~~~~~~~~~
	unsigned char outbuf[2];
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[1];
	
	messages[0].addr = this->slv_addr;
	messages[0].flags = 0;
	messages[0].len = sizeof(outbuf);
	messages[0].buf = outbuf;
	
	//First byte indicates the register we will write to
	outbuf[0] = rgstr;
	
	//Second byte indicates the value we want to write
	outbuf[1] = data_byte;
	
	//Transfer to the kernel and verify that it worked
	packets.msgs = messages;
	packets.nmsgs = 1;
	if(ioctl(this->i2c_fd,I2C_RDWR, &packets) < 0)
	{
		perror("Unable to write data, whoops");
		return 1;
	}
	return 0;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}

void I2C_Slave::i2cStreamInSeq()
{
	/**
		Function to stream in the data into the stream_in deque one byte at a time correponding to the registers in registers2read
			
	*/
	int sz = register2read.size();
	if(sz ==0) {
		perror("No registers to read from");
		return;
	}
	for(int i = 0; i<sz; i++)
	{
		auto d_byte = this->i2cRead8(this->register2read.front());
		this->register2read.pop_front();
		this->stream_in.push_back(d_byte);
	}
}
void I2C_Slave::i2cStreamOutSeq()
{
	/**
		Function to stream out the commands in the stream_out deque one by one correponding to the registers in registers2write
	
	*/
	int sz = register2write.size();
	if(sz ==0) {
		perror("No registers to write to");
		return ;
	}
	for(int i = 0; i<sz; i++)
	{
		this->i2cWrite8(this->register2write.front(), this->stream_out.front());
		this->register2write.pop_front();
		this->stream_out.pop_front();
	}
}

void I2C_Slave::i2cStreamInBlock(unsigned char numBytes, unsigned char rgstr, unsigned char ptr_cmd, unsigned char blk_cmd)
{
	/**
		Function that streams in a block of data given a pointer command placed at specified rgstr.
		
		@param rgstr 	The character value of the register to start the block read.
		@param  ptr_cmd	The command that initiates the pointer to the register 
		@param blk_cmd	The command to initate the block read
	*/
	this->i2cWrite8(ptr_cmd, rgstr);														
	int sz = (int)numBytes;
	if(sz ==0) {
		perror("No registers to read from");
		return;
	}
	 
	unsigned char inbuf[sz], outbuf[2];
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];
	
//~~~~~~~~~~~~~~ Enter Code Here~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
	Use this area to add structure and contents to the packets and messages structs
		outbuf --> output data 
		inbuf --> input data
*/ 
	//First, we "dummy write" 0 bits to the register we want to read from
	outbuf[0] = blk_cmd;
	outbuf[1] = numBytes;
	
	messages[0].addr = this->slv_addr;
	messages[0].flags = 0;
	messages[0].len = sizeof(outbuf);
	messages[0].buf = outbuf;

	//Data will be returned in this structure
	messages[1].addr = this->slv_addr;
	messages[1].flags = I2C_M_RD;
	messages[1].len = sizeof(inbuf);
	messages[1].buf = inbuf;

	//Send the request to the kernel and get the result back
	packets.msgs = messages;
	packets.nmsgs = 2;
	if(ioctl(this->i2c_fd, I2C_RDWR, &packets) < 0) 
	{
		perror("Unable to write block:");
		return;
	}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	for(int i = 0; i<sz; i++)
	{
		this->stream_in.push_back(inbuf[i]);
	}
}

void I2C_Slave::i2cStreamOutBlock(unsigned char rgstr, unsigned char ptr_cmd, unsigned char blk_cmd) 
{
	/**
		Function that streams out a block of commands given a pointer command placed at specified rgstr.
		
		@param rgstr t	The character value of the register to start the block write.
		@oaram  ptr_cmd	The command that initiates the pointer to the register 
		@param blk_cmd	The command to initate the block write
	*/
	this->i2cWrite8(ptr_cmd, rgstr);									//Send an 8-bit command code to tell the slave to expect a block write
	int sz = register2write.size();
	if(sz ==0) {
		perror("No registers to read from");
		return;
	}

	unsigned char outbuf[2+sz];
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[1];
	// Need to store in deque
	//Need to do deque.pop_front() stuff to get rid of it 
	
//~~~~~~~~~~~~~~ Enter Code Here~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
	Use this area to add structure and contents to the packets and messages structs
		outbuf --> output data 
*/ 
	//First byte indicates the register we will write to
	outbuf[0] = blk_cmd;
	outbuf[1] = sz;
	
	//Remaining bytes are used to store the info we want to send
	for (int i = 2; i < sizeof(outbuf); i++)
	{
		outbuf[i] = stream_out.front();									//Grab the first message in stream_out
		stream_out.pop_front(); 										//Remove the first message in the queue
	}
	
	messages[0].addr = this->slv_addr;
	messages[0].flags = 0;
	messages[0].len = sizeof(outbuf);
	messages[0].buf = outbuf;
	
	//Transfer to the kernel and verify that it worked
	packets.msgs = messages;
	packets.nmsgs = 1;
	if(ioctl(this->i2c_fd,I2C_RDWR, &packets) < 0)
	{
		perror("Unable to send data");
		return;
	}
	
	
	return;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			
}


