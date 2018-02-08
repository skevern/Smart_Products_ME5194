//==============================================================================
// ---------------------------------------------------------- lab2_post_Q2.cpp -----------------------------------------------------------------
//==============================================================================
/**
	_filename: lab2_post_Q2.cpp
	_description: This main program employs the Microchip 23K256 SPI Bus Low-Power Serial SRAM
	to demonstrate the use of the SPI communication protocol. The SRAM is initated as a 
	SPI_Slave object from the imported header file "SPI_Slave.h" which utilizes the Linux based 
	I/O control functions, specifically the  " ioctl(...) "  function. The two possible SPI configurations
	are either SPI0_CS0 or SPI0_CS1 which correspond to the SPI  0 bus with the 0 chip select, or 
	the SPI 0 bus with the 1 chip select. On the raspberry pi 3B this corresponds with the pin 
	configuration :
			-------------------------------------------------
			| 	MOSI  ---> Pin 19		|
			| 	MISO  ---> Pin 21		|
			|	CLK	   ---> Pin 23		|
			|	CE0/1 ---> Pin 24/25	|
			-------------------------------------------------
	The program shows the simple memory command of reading the first address, writing to it, then
	reading it again showing how our value has been stored. The address space (0x0000 - 0x7FFF) 
	of the SRAM is accessed with a 16-bit pointer. The memory is allocated by with word sizes of 
	8-bit and page sizes of 32-bytes with a total of 1024 pages.
	
	The word size of the SPI transfer is by default 8-bit and therefore address pointers must be 
	broken up into two 8-bit commands.The sequence of the commands for the modes are where X
	corresponds to a clock cycle were the line is held at high impedance:
	-----------------------------------------------------MOSI SIGNALS -----------------------------------------------------------------------------------------------------------------------------------------------------
	~~~ READ MODE:				{RDSR}
	~~~ WRITE MODE: 			{WRSR, MODE}
	~~~ READ BYTE MODE: 		{READ, ptr_MSB, ptr_LSB}		
	~~~ WRITE BYTE MODE: 		{WRITE,  ptr_MSB, ptr_LSB, word}
	~~~ READ PAGE MODE: 		{READ, ptr_MSB, ptr_LSB}
	~~~ WRITE PAGE MODE: 		{WRITE,, ptr_MSB, ptr_LSB, pageX_wordY, page_X_word(Y+1), ..., pageX_word31, pageX_word0, ...} 
	~~~ READ SEQUENCE MODE:	{READ, ptr_MSB, ptr_LSB}
	~~~ WRITE SEQUENCE MODE: 	{WRITE, ptr_MSB, ptr_LSB, pageX_wordY, page_X_word(Y+1), ..., pageX_word31, page(X+1)_word0, ...} 
	-----------------------------------------------------MISO SIGNALS ----------------------------------------------------------------------------------------------------------------------------------------------------
	~~~ READ MODE:				{X, MODE}
	~~~ WRITE MODE: 			{X, X}
	~~~ READ BYTE MODE: 		{X, X, X, word}		
	~~~ WRITE BYTE MODE: 		{X,  X, X, X}
	~~~ READ PAGE MODE: 		{X, X, X, pageX_wordY, page_X_word(Y+1), ..., pageX_word31, pageX_word0, ...} 
	~~~ WRITE PAGE MODE: 		{X, X, X, ..., X.} 
	~~~ READ SEQUENCE MODE:	{X, X, X, pageX_wordY, page_X_word(Y+1), ..., pageX_word31, page(X+1)_word0, ...} 
	~~~ WRITE SEQUENCE MODE: 	{X, X, X, ..., X} 
======================================================================================================================
	@author Dylan DeSantis
	@version 1.0.0
*/
#include "SPI_Slave.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include<math.h>

int main()
{
	int spi1_fd = open(SPI0_CS0, O_RDWR );
	unsigned char result;
	unsigned char  READ =0x03; // read byte from memory command
	unsigned char WRITE = 0x02; // write byte to memory command
	unsigned char READ_STATUS= 0x05;
	unsigned char WRITE_STATUS = 0x01;
	unsigned char BYTE_MODE = 0x00;
	unsigned char PAGE_MODE = 0x02;
	unsigned char SEQ_MODE= 0x01;


	unsigned char ptr_addr_1_MSB = 0x00;// 16-8
	unsigned char ptr_addr_1_LSB= 0x01;// 7-0
	unsigned char value = 0xA1;
	
	// There are 4 separate transfers: 1) WRITE_STATUS  2) READ 3) WRITE  4) READ
	unsigned char commands[] = {WRITE_STATUS, BYTE_MODE,
							     READ, ptr_addr_1_MSB, ptr_addr_1_LSB, 
							     WRITE, ptr_addr_1_MSB, ptr_addr_1_LSB, value,
							     READ, ptr_addr_1_MSB,  ptr_addr_1_LSB };
							  
	// There are 12 total commands being sent over MOSI with the commands allocated 
	// for each transfer as: Transfer 1 = 2 cmds, Transfer 2 = 3 cmds, Transfer 3 = 4 cmds, Transfer 4  = 3 cmds.					  
	unsigned char tx_lens_1[] = {0x2, 0x3, 0x4, 0x3}; 
	unsigned char rx_lens_1[] = {0x0, 0x1, 0x0,0x1};
	
	int mode = 0; //SPI MODE
	int cs_delay = 1; //Chip select delay (not used)
	int max_speed = 16*pow(10,6); // max transmit speed in Hz (not used)
//=====================================================================
// ------------------------------------------------ Initializing SPI Device ----------------------------------------------------
//=====================================================================
	SPI_Slave SRAM(spi1_fd, mode,cs_delay,max_speed);
	SRAM.spiBegin();
	SRAM.spiPutBlkCmds(rx_lens_1, tx_lens_1, commands, 12, 4);
//=====================================================================
// -------------------------------------------- Sending the SPI Transfers  -----------------------------------------------
//=====================================================================	
	SRAM.spiTransfer(32, 32, SEQ);
	for( int i = 0 ; i<SRAM.stream_in.size(); i++)
	{
	printf(" %X \n", (int)SRAM.stream_in[i]);
	}
	close(spi1_fd);
	//PmodIA.~I2C_Slave();// invoke the destructor and unmap the address
	return 0;
}

