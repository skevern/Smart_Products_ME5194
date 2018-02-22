// Filename: lab2_Q1.cpp 
// Compile using the terminal command: 
// -----------------------------------------------------------------------------------------------------------------------------------------
//       sudo g++ -o lab2_Q1 lab2_Q1.cpp lab2_gpio.cpp lab2_gpio.h 
// -----------------------------------------------------------------------------------------------------------------------------------------
#include "lab2_gpio.h"
#include <stdio.h>
#include<iostream>

using namespace std;

int main()
{
	// Start the Pin control session for memory mapping  
	lab2_gpio Sess; // enables the constructor

/*		int pin_num = 17; //point to control 
		Sess.pinMode(pin_num, OUTPUT);										// set the pinmode for output on pin 27
		int pin_mode_status = Sess.pinModeRead(pin_num,OUTPUT); 			//Check if the pinmode matches our desired pinmode
		cout << "\nPinmode status is: (should be 1 to match) " << pin_mode_status << endl;
		
		Sess.digitalWrite(pin_num, LOW); 									// initally write set the pin to low 
		int pin_val = Sess.digitalRead(pin_num); 							// read the value of the pin 
		cout << "Value of Pin (should be low)" << pin_num << " is: " << pin_val << endl;	
		
		Sess.digitalWrite(pin_num, HIGH); 		//write high to pin 27 
		int pin_val_2 = Sess.digitalRead(pin_num);		// read the pin
		cout<<"Value of pin (should be high)"<< pin_num << " is  " <<  pin_val_2 <<std::endl;
*/
	
	
	//Test a single pin
	int pin_num = 4;
	Sess.pinMode(pin_num, OUTPUT);										//Set the pin to output mode
	int pin_mode_status = Sess.pinModeRead(pin_num,OUTPUT); 			//Check if the pinmode matches our desired pinmode
	cout << "\nPinmode status is: (should be 1 to match) " << pin_mode_status << endl;
	
	Sess.digitalWrite(pin_num, LOW); 									// initally write set the pin to low 
	int pin_val = Sess.digitalRead(pin_num); 							// read the value of the pin 
	cout << "Value of Pin (should be low)" << pin_num << " is: " << pin_val << endl;

	Sess.digitalWrite(pin_num, HIGH); 				//write high to pin_num 
	int pin_val_2 = Sess.digitalRead(pin_num);		// read the pin
	cout<<"Value of pin (should be high)"<< pin_num << " is  " <<  pin_val_2 <<std::endl;
	

	
	Sess.~lab2_gpio();// invoke the destructor and unmap the address
	return 0;
}
