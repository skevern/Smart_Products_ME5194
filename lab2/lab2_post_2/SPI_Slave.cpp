/**
_filename: SPI_Slave.cpp
_description: Definition file for a SPI slave class.
//=====================================================================
// ------------------------------------ SPI Slave Class Definitions ------------------------------------------------
// ____________________________________________________________________________
// xxxx This class defines a general layout for slave devices connected to the 
// xxxx master. Use this class a parent class for specific slave devices with which
// xxxx you communicate via SPI
//=====================================================================
@author Dylan DeSantis
@version 1.0.0
*/
#include "SPI_Slave.h"
#include <iostream>
//=====================================================================
// -------------------------------------- METH: Open/closing and utilities -------------------------------------------
//=====================================================================
int SPI_Slave::spiBegin()
{
	/**
		Function to set the slave device and modes of the SPI	
	*/
	int mode = this->spi_mode;
	if(mode == 0) ioctl(this->spi_fd, SPI_IOC_WR_MODE, SPI_MODE_0);
	if(mode == 1) ioctl(this->spi_fd, SPI_IOC_WR_MODE, SPI_MODE_1);
	if(mode == 2) ioctl(this->spi_fd, SPI_IOC_WR_MODE, SPI_MODE_2);
	if(mode == 3) ioctl(this->spi_fd, SPI_IOC_WR_MODE, SPI_MODE_3);
}
int SPI_Slave::spiEnd()
{
	/**
		Function to end any concurent threads (optional)
	*/
	
	
}
void SPI_Slave::spiDelay(int delay_time)
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
void SPI_Slave::spiPutBlkCmds( unsigned char rxlens[], unsigned char txlens[],unsigned char cmds[], int numCmds,int numXfers)
{
	/**
		Function for placing the transfer data into (-->) the deques: cmd --> stream_out, rxlens --> receive_len, txlens --> transmit_len
		
		@param rxlens an array of size numXfers, with each entry's value being the number of commands that the corresponding transfer will receive	
		@param txlens an array of size numXfers, with each entry's value being the number of commands that the corresponding transfer will transmit
		@param cmds an array of size numCmds, with each entry's value being the command with word size corresponding to the SPI bits per word attribute
		@param numCmds an integer value denoting the total number of transfers
		@param numXfers an integer value denoting the total number of transfers
	*/
	for (int i = 0; i<numCmds; i++)
	{
		this->stream_out.push_back(cmds[i]);
	} 
	for (int i = 0; i<numXfers; i++)
	{
		this->transmit_len.push_back((int)txlens[i]);
		this->receive_len.push_back((int)rxlens[i]);
	}
}
	
void SPI_Slave::spiClearQueues()
{
	/**
		Function to clear the deques: stream_in, stream_out, transmit_len, reviece_len.	
	*/
	this->stream_in.clear();
	this->stream_out.clear();
	this->transmit_len.clear();
	this->receive_len.clear();
}

//=====================================================================
// -------------------------------------- METH: Read/ Write  ----------------------------------------------------------------
//=====================================================================
void SPI_Slave::spiTransfer(int outbuf_size, int inbuf_size, xferTiming xfer_type)
{
	/**
		Function for completing a transfer of data over SPI. 		
		
		@param outbuf_size an integer value denoting the size of the output buffer	
		@param inbuf_size an integer value denoting the size of the input buffer (receiving data) 		
		@param xfer_type an emun type value declaring if transmit and receiving messages occur synchronously or sequentially 
	*/
//~~~~~~~~~~~~~~~~~~~Enter Code Here ~~~~~~~~~~~~~~~~~~~~~~~~~
/*
	The function should work in the following way:
			1. Check to see if there are any contents in the transmit_len deque.
			2. setup conditions to handle both synchronous and sequential data transmission
			3.. for each transfer (each entry in transmit_len)
				a.  fill the output buffer with the correct amount of commands.
				b. Setup a receive buffer if  there any messages to acquire
				c. transmit the message using 
					ioctl(this->spi_fd, SPI_IOC_MESSAGE(numMess), xfer)
				d. place contents of inbuf into stream_in if there is any incoming messages
				
	Find examples at: https://github.com/01org/edison-linux/blob/master/Documentation/spi/spidev_fdx.c
	Other Resources at:
	1. http://en.cppreference.com/w/cpp/string/byte/memset
	2. https://www.emcraft.com/stm32f429discovery/accessing-spi-devices-in-linux
	3. https://github.com/spotify/linux/blob/master/include/linux/spi/spidev.h
	4. https://www.kernel.org/doc/Documentation/spi/spidev
*/



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

}


