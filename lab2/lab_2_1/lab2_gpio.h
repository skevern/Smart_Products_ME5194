// LAB2 Header file 
// Filename: lab2_gpio.h
#ifndef LAB2_GPIO
#define LAB2_GPIO

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#define PERPH_BASE 0x3F000000
#define GPIO_BASE 0x200000
#define GPIO_READ0 0x0034
#define GPIO_READ1 0x0038
#define GPIO_HIGH0 0x001C
#define GPIO_HIGH1 0x0020
#define GPIO_LOW0 0x0028
#define GPIO_LOW1 0x002C
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE-1)
#define MEM_PATH "/dev/mem"
#define GPIO_PATH "/dev/gpiomem"		
#define GP_MASK 0x7

// Complete the addressing using the tech spec for RPi3
//-----------------------------------------------------------------------------------------------------------------------------------------
// Replace the NN in each of the below addresses
//-----------------------------------------------------------------------------------------------------------------------------------------
#define GPFSEL0	0x0000
#define GPFSEL1 0x0004
#define GPFSEL2	0x0008
#define GPFSEL3	0x000C
#define GPFSEL4	0x0010
#define GPFSEL5	0x0014
//-----------------------------------------------------------------------------------------------------------------------------------------

typedef enum{ HIGH = 1, LOW = 0} outValue ;

typedef enum {
	INPUT =  0x0,
	OUTPUT = 0x1,
	ALT0 = 0x4,
	ALT1 = 0x5,
	ALT2 = 0x6,
	ALT3 = 0x7,
	ALT4 = 0x3,
	ALT5 = 0x2
	} pinModes;

class lab2_gpio
{
private:
	void *ptr_base;
	int memfd;
public:
	int  pinMode(int, pinModes);
	int  pinModeRead(int, pinModes);
	int  digitalRead(int);
	int  digitalWrite(int, outValue);
	void  print_binary(unsigned long);
	
	lab2_gpio()
	{
		// Complete offset + base for GPIO
		off_t gpio_base = ((off_t)(PERPH_BASE + GPIO_BASE));// replace xxx : base address for GPIO
		// Access the driver using the open() function
		memfd = open(MEM_PATH,O_RDWR|O_SYNC);// replace xxx: open driver
		if (memfd<0)  printf("Cannot open");
		// map the addresses using the mmap() function
		ptr_base = mmap(NULL, MAP_SIZE, (PROT_READ|PROT_WRITE), MAP_SHARED, memfd, gpio_base);// replace xxx: Mapping
	}
	~lab2_gpio()
	{
		//Destructor for the lab2_gpio function 
		munmap(ptr_base,MAP_SIZE);
		close(memfd);
	}	
};
#endif


