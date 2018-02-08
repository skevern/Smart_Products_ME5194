// Filename: lab2_gpio.cpp
#include "lab2_gpio.h"
#include <iostream>
#include "math.h"
#include <bitset>

// ------------------------------------ LAB 2 Question 1 Class Definitions ------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------
void lab2_gpio::print_binary(unsigned long input)
{
	std::bitset<32> x(input);
	std::cout << x << std::endl; 
}

int lab2_gpio::pinMode(int pinNum, pinModes pMode)
{
// Purpose: This function sets the pin mode for the desired pin mode
//pinMode is an integer number for the pin we want to change
//pMode is a 3-bit sequence that must be inserted into our memory address
int pos = 0; 
void *temp_ptr;
off_t gpf_select;

	if (pinNum<10)
	{
		// ---------------------------ADD CODE HERE for pins 0-9 -------------------------------------------
		gpf_select = (off_t)GPFSEL0;
		pos = (pinNum)*3;
		
		// -----------------------------------------------------------------------------------------------------------------------
	}
	else if(9<pinNum && pinNum<20)
	{
		// ---------------------------ADD CODE HERE for pins 10-19 ----------------------------------------
		gpf_select = (off_t)GPFSEL1;
		pos = (pinNum - 10)*3;
		// -----------------------------------------------------------------------------------------------------------------------
	}
	else if(19<pinNum && pinNum<30)
	{
		// ---------------------------ADD CODE HERE for pins 20-29 ----------------------------------------
		gpf_select = (off_t)GPFSEL2;
		pos = (pinNum - 20)*3;
		// -----------------------------------------------------------------------------------------------------------------------
	}
	else	if(29<pinNum && pinNum<40)
	{
		// ---------------------------ADD CODE HERE for pins 30-39 ----------------------------------------
		gpf_select = (off_t)GPFSEL3;
		pos = (pinNum - 30)*3;
		// -----------------------------------------------------------------------------------------------------------------------
	}
	else	if(39<pinNum && pinNum<50)
	{
		// ---------------------------ADD CODE HERE for pins 40-49 ----------------------------------------
		gpf_select = (off_t)GPFSEL4;
		pos = (pinNum - 40)*3;
		// -----------------------------------------------------------------------------------------------------------------------
	}
	else	if(49<pinNum && pinNum<54)
	{
		// ---------------------------ADD CODE HERE for pins 50-53 ----------------------------------------
		gpf_select = (off_t)GPFSEL5;
		pos = (pinNum - 50)*3;
		// -----------------------------------------------------------------------------------------------------------------------
	}
	else{
		printf("Pin number  %d out of range\n", pinNum);
		return -1;
	}
	
	//Bulk of the work is done here

	temp_ptr = ptr_base + (off_t)gpf_select;							//Offset the memory location by the location of each register
	unsigned long read_result = *((unsigned long*)temp_ptr); 			//Store the current configuration of our 32-bit register	
	unsigned long read_result_2 = pMode<<pos;							// read result 2 looks like 00000000000000 001 00000000000000         
	unsigned long bitmask = ~(GP_MASK<<pos);							//create a mask to have pMode in place of the pin pos slots "111"
	unsigned long filtSelect = read_result & bitmask; 					//AND 
	unsigned long newSelect = filtSelect|read_result_2;					//OR
	*((unsigned long*)temp_ptr) = newSelect;							//Set the current configuration to newSelect
	return 0;
}

int lab2_gpio::pinModeRead(int pinNum, pinModes pMode)
{
// Purpose: This function reads the current pin mode of pinNum and compares it to the 
//		   desired pin mode pMode, returns 1 if it matches and -1 if it does not
	unsigned long desired_result, read_result;
	int pos = 0; 
	void *temp_ptr;
	off_t gpf_select;
	desired_result = pMode;  
	
	if (pinNum<10)
	{
		// ---------------------------ADD CODE HERE for pins 0-9 -------------------------------------------
		gpf_select = (off_t)GPFSEL0;
		pos = (pinNum)*3;
		// -----------------------------------------------------------------------------------------------------------------------
		
		if(desired_result == read_result) return 1;
	}
	else if(9<pinNum && pinNum<20)
	{
		// ---------------------------ADD CODE HERE for pins 10-19 ----------------------------------------
		gpf_select = (off_t)GPFSEL1;
		pos = (pinNum - 10)*3;
		// -----------------------------------------------------------------------------------------------------------------------
	}
	else if(19<pinNum && pinNum<30)
	{
		// ---------------------------ADD CODE HERE for pins 20-29 ----------------------------------------
		gpf_select = (off_t)GPFSEL2;
		pos = (pinNum - 20)*3;
		// -----------------------------------------------------------------------------------------------------------------------
		if(desired_result == read_result) return 1;
	}
	else	if(29<pinNum && pinNum<40)
	{
		// ---------------------------ADD CODE HERE for pins 30-39 ----------------------------------------
		gpf_select = (off_t)GPFSEL3;
		pos = (pinNum - 30)*3;
		// -----------------------------------------------------------------------------------------------------------------------
		if(desired_result == read_result) return 1;
	}
	else	if(39<pinNum && pinNum<50)
	{
		// ---------------------------ADD CODE HERE for pins 40-49 ----------------------------------------
		gpf_select = (off_t)GPFSEL4;
		pos = (pinNum - 40)*3;
		// -----------------------------------------------------------------------------------------------------------------------
		if(desired_result == read_result) return 1;
	}
	else	if(49<pinNum && pinNum<54)
	{
		// ---------------------------ADD CODE HERE for pins 50-53 ----------------------------------------
		gpf_select = (off_t)GPFSEL5;
		pos = (pinNum - 50)*3;
		// -----------------------------------------------------------------------------------------------------------------------
		if(desired_result == read_result) return 1;
	}
	else{
		printf("Pin number  %d out of range\n", pinNum);
		return -1;
	}			
	//Bulk of the work is done here
		temp_ptr = ptr_base + (off_t)gpf_select;							//Offset the memory location by the location of each register
		read_result = *((unsigned long*)temp_ptr); 							//Store the current configuration of our 32-bit register	
		unsigned long read_result_2 = pMode<<pos;							// read result 2 looks like 00000000000000 001 00000000000000         
		unsigned long bitmask = ~(GP_MASK<<pos);							//create a mask to have pMode in place of the pin pos slots "111"
		unsigned long filtSelect = read_result & bitmask; 					//AND 
		unsigned long newSelect = filtSelect|read_result_2;					//OR	
		
		//Check if our desired result = the new result
		if(newSelect == read_result) return 1;
		else return -1;
									
}


int lab2_gpio::digitalRead(int pinNum)
{
// Purpose: This function reads the value of the pin pinNum and returns 1 if high and 
//		   0 if low, if it is neither then return -1.
	unsigned long bitHigh, pinNvalue, read_result, read_result_2;
	int pos;
	void *temp_ptr;
	off_t gpf_select;
	
	if (0<= pinNum && pinNum<32)
	{
		// ---------------------------ADD CODE HERE for pins 0-31 -------------------------------------------
		pos = pinNum;
		gpf_select = (off_t)GPIO_READ0;
		temp_ptr = ptr_base + (off_t)gpf_select;							//Offset the memory location by the location of each register
		read_result = *((unsigned long*)temp_ptr); 							//Store the current configuration of our 32-bit register
		read_result_2 = 1UL <<pos;												// read result 2 looks like 00000000000000 001 00000000000000 
		unsigned long bitmask = ~(1UL<<pos);									//create a mask to have pMode in place of the pin pos slots "111"
		unsigned long filtSelect = read_result & bitmask; 					//AND
		unsigned long newSelect = filtSelect|read_result_2;					//OR
		/*std::cout << "In the digitalWrite function" << std::endl;
		std::cout << "The value of read_result is: "; print_binary(read_result);
		std::cout << "The value of read_result_2 is: "; print_binary(read_result_2);
		std::cout << "The value of bitmask is: "; print_binary(bitmask);
		std::cout << "The value of filtSelect is: "; print_binary(filtSelect);
		std::cout << "The value of newSelect is: "; print_binary(newSelect); std::cout << "\n" << std::endl;*/
		
		if (newSelect == read_result) return 1; 
		else return 0; 
		
		// -----------------------------------------------------------------------------------------------------------------------
		/*if (pinNvalue == bitHigh)  return 1;
		else if(pinNvalue == 0) return 0;
		else{
			printf("Read value of pin %d is %X\n", pinNum, pinNvalue);
			return -1;
		} */
		
	}
	else if(31< pinNum && pinNum<54)
	{
		// ---------------------------ADD CODE HERE for pins 32-53 -----------------------------------------
		pos = (pinNum - 31);
		gpf_select = (off_t)GPIO_READ1;
		temp_ptr = ptr_base + (off_t)gpf_select;							//Offset the memory location by the location of each register
		read_result = *((unsigned long*)temp_ptr); 							//Store the current configuration of our 32-bit register
		read_result_2 = 1UL <<pos;												// read result 2 looks like 00000000000000 001 00000000000000 
		unsigned long bitmask = ~(1UL<<pos);									//create a mask to have pMode in place of the pin pos slots "111"
		unsigned long filtSelect = read_result & bitmask; 					//AND
		unsigned long newSelect = filtSelect|read_result_2;					//OR
		
		/*std::cout << "In the digitalRead function" << std::endl;
		std::cout << "The value of read_result is: "; print_binary(read_result);
		std::cout << "The value of read_result_2 is: "; print_binary(read_result_2);
		std::cout << "The value of bitmask is: "; print_binary(bitmask);
		std::cout << "The value of filtSelect is: "; print_binary(filtSelect);
		std::cout << "The value of newSelect is: "; print_binary(newSelect); std::cout << "\n" << std::endl;*/
		
		if (newSelect == read_result) return 1; 
		else return 0; 
		
		// -----------------------------------------------------------------------------------------------------------------------
		/* if (pinNvalue == bitHigh)  return 1;
		else	if(pinNvalue == 0) return 0;
		else{
			printf("Read value of pin %d is %X pinNvalue\n", pinNum, pinNvalue);
			return -1;
		} */
	}
	else{
		printf("Pin number  %d out of range\n", pinNum);
		return -1;
	}
}

int lab2_gpio::digitalWrite(int pinNum, outValue out_v)
{
// Purpose: This function first checks that the pin mode of the pin you wish to write to 
// 		   is set to OUTPUT. It then writes high or low to the pin.
	unsigned long bitHigh, pinNvalue, read_result, read_result_2;
	int pos;
	void *temp_ptr;
	off_t gpf_select;
	
	int status = lab2_gpio::pinModeRead(pinNum, OUTPUT);
	if(status == -1) 
	{
		printf("Pin mode is not in output mode\n");
		return -1;
	}
	if (0<= pinNum && pinNum<32)
	{
		if(out_v == HIGH)
		{
		// ---------------------------ADD CODE HERE for pins 0-31 -------------------------------------------
		pos = pinNum;
		gpf_select = (off_t)GPIO_HIGH0;
		temp_ptr = ptr_base + (off_t)gpf_select;							//Offset the memory location by the location of each register
		read_result = *((unsigned long*)temp_ptr); 							//Store the current configuration of our 32-bit register
		read_result_2 = 1UL <<pos;												// read result 2 looks like 00000000000000 001 00000000000000 
		unsigned long bitmask = ~(1UL<<pos);									//create a mask to have pMode in place of the pin pos slots "111"
		unsigned long filtSelect = read_result & bitmask; 					//AND
		unsigned long newSelect = filtSelect|read_result_2;					//OR
		*((unsigned long*)temp_ptr) = newSelect;							//Set the current configuration to newSelect

		//printf("\nRead Result for write function is: %i", read_result);
		
		/*std::cout << "In the digitalWrite function" << std::endl;
		std::cout << "The value of read_result is: "; print_binary(read_result);
		std::cout << "The value of read_result_2 is: "; print_binary(read_result_2);
		std::cout << "The value of bitmask is: "; print_binary(bitmask);
		std::cout << "The value of filtSelect is: "; print_binary(filtSelect);
		std::cout << "The value of newSelect is: "; print_binary(newSelect); std::cout << "\n" << std::endl;*/
		
		// -----------------------------------------------------------------------------------------------------------------------
			return 1;
		}
		else if(out_v == LOW)
		{
		// ---------------------------ADD CODE HERE for pins 0-31-------------------------------------------
		pos = pinNum;
		gpf_select = (off_t)GPIO_LOW0;
		temp_ptr = ptr_base + (off_t)gpf_select;							//Offset the memory location by the location of each register
		read_result = *((unsigned long*)temp_ptr); 							//Store the current configuration of our 32-bit register
		read_result_2 = 1UL <<pos;												// read result 2 looks like 00000000000000 001 00000000000000 
		unsigned long bitmask = ~(1UL<<pos);									//create a mask to have pMode in place of the pin pos slots "111"
		unsigned long filtSelect = read_result & bitmask; 					//AND
		unsigned long newSelect = filtSelect|read_result_2;					//OR
		*((unsigned long*)temp_ptr) = newSelect;							//Set the current configuration to newSelect

		//printf("\nRead Result for write function is: %u", read_result);
		
		/*std::cout << "In the digitalWrite function" << std::endl;
		std::cout << "The value of read_result is: "; print_binary(read_result);
		std::cout << "The value of read_result_2 is: "; print_binary(read_result_2);
		std::cout << "The value of bitmask is: "; print_binary(bitmask);
		std::cout << "The value of filtSelect is: "; print_binary(filtSelect);
		std::cout << "The value of newSelect is: "; print_binary(newSelect); std::cout << "\n" << std::endl;*/
		
		// -----------------------------------------------------------------------------------------------------------------------
			return 1;
		}
	}
	else if(31< pinNum && pinNum<54)
	{
		if(out_v == HIGH)
		{
		// ---------------------------ADD CODE HERE for pins 32-53 -------------------------------------------
		pos = (pinNum-31);
		gpf_select = (off_t)GPIO_HIGH1;
		temp_ptr = ptr_base + (off_t)gpf_select;							//Offset the memory location by the location of each register
		read_result = *((unsigned long*)temp_ptr); 							//Store the current configuration of our 32-bit register
		read_result_2 = 0UL <<pos;												// read result 2 looks like 00000000000000 001 00000000000000 
		unsigned long bitmask = ~(0UL<<pos);									//create a mask to have pMode in place of the pin pos slots "111"
		unsigned long filtSelect = read_result & bitmask; 					//AND
		unsigned long newSelect = filtSelect|read_result_2;					//OR
		*((unsigned long*)temp_ptr) = newSelect;							//Set the current configuration to newSelect
		
		/*std::cout << "In the digitalWrite function" << std::endl;
		std::cout << "The value of read_result is: "; print_binary(read_result);
		std::cout << "The value of read_result_2 is: "; print_binary(read_result_2);
		std::cout << "The value of bitmask is: "; print_binary(bitmask);
		std::cout << "The value of filtSelect is: "; print_binary(filtSelect);
		std::cout << "The value of newSelect is: "; print_binary(newSelect); std::cout << "\n" << std::endl;*/
		
		// -----------------------------------------------------------------------------------------------------------------------
			return 1;
		}
		else if(out_v == LOW)
		{
		// ---------------------------ADD CODE HERE for pins 32-53-------------------------------------------
		pos = (pinNum-31);
		gpf_select = (off_t)GPIO_LOW1;
		temp_ptr = ptr_base + (off_t)gpf_select;							//Offset the memory location by the location of each register
		read_result = *((unsigned long*)temp_ptr); 							//Store the current configuration of our 32-bit register
		read_result_2 = 0UL <<pos;												// read result 2 looks like 00000000000000 001 00000000000000 
		unsigned long bitmask = ~(0UL<<pos);									//create a mask to have pMode in place of the pin pos slots "111"
		unsigned long filtSelect = read_result & bitmask; 					//AND
		unsigned long newSelect = filtSelect|read_result_2;					//OR
		*((unsigned long*)temp_ptr) = newSelect;							//Set the current configuration to newSelect
		
		/*std::cout << "In the digitalWrite function" << std::endl;
		std::cout << "The value of read_result is: "; print_binary(read_result);
		std::cout << "The value of read_result_2 is: "; print_binary(read_result_2);
		std::cout << "The value of bitmask is: "; print_binary(bitmask);
		std::cout << "The value of filtSelect is: "; print_binary(filtSelect);
		std::cout << "The value of newSelect is: "; print_binary(newSelect); std::cout << "\n" << std::endl;*/
		
		// -----------------------------------------------------------------------------------------------------------------------
			return 1;
		}
	}
	else{
		printf("Pin number  %d out of range \n", pinNum);
		return -1;
	}
}


