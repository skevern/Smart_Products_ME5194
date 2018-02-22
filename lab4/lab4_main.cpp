/**
//====================================================================
//                  	lab4_main.cpp
//====================================================================

	_description: This is the header file to control the robotic arm, Dobot Magician, 
		using the raspberry pi 3. We connect the RPi to the Dobot using the usb cable
		that comes with the Dobot. Utilizing the communication protocol of developed
		by the Dobot company, we will package the different commands and setup
		procedure into a more compact and usable class called Robot. Make sure 
		have installed the wiringPi library. To connect 
		with the Dobot, all you need to do is 
			1. turn off the Dobot
			2. Plug in the USB 3 cable to the Pi and the Dobot.
			3. Turn on the Dobot. 
			4. Wait about 30 seconds perform trying to run any programs
			5. Begin your program.

===============To compile enter the following command ================
sudo g++ -o lab4 lab4_main.cpp Message.h Message.cpp  
Packet.h Packet.cpp ProtocolDef.h Protocol.h Protocol.cpp ProtocolID.h 
RingBuffer.h RingBuffer.cpp Robot.h Robot.cpp 
command.h command.cpp -lwiringPi
========================References===============================
	https://www.dobot.cc/dobot-magician/specification.html
	https://www.dobot.cc/download/dobot-communication-protocol-v1-0-4/
	https://www.dobot.cc/download/dobot-magician-api-v1-1-1/
=================================================================
	@author 		Dylan DeSantis
	@date		2/16/2018
	@version	1.0.0
*/






#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>        
#include<errno.h>
#include<math.h>
#include<signal.h>
#include<unistd.h>
#include "wiringPi.h"
#include "wiringSerial.h"
#include<array>

#include "Protocol.h"
#include "command.h"
#include "Robot.h"
//Set Serial TX&RX Buffer Size
#define SERIAL_TX_BUFFER_SIZE 64
#define SERIAL_RX_BUFFER_SIZE 256
extern int fd;

int fd=serialOpen( UART_PATH, BAUD_RATE);

int main() 
{
	//	Setup
	int fd = serialOpen( UART_PATH, BAUD_RATE);
	Robot Dobot;
	Dobot.setRxInterrupt(100);
	Dobot.initRAM();
	ProtocolInit();
	Dobot.setInitParams();
	
	//Dobot.goToHome();
	

    	
        printf("\r\n======Enter Main Routine ======\r\n");
        /**
          Here you will program the dobot to pick up the
          grid of table tennis balls and place them into the
          container. You may use the provided functions in
          the Robot class. The pick and place operation should
          navigate to the location of the ball, drop down onto it
          to acquire a secure seal and then turn on the pump.
          The robot should wait for a moment to let the pump
          to obtain a vacuum. Then you you should navigate to a
          position over the box and turn off the pump. Repeat this
          until all the balls are placed into the box.
         **/
    //Dobot.goToHome();
    
    int ypos[2] = { -20, 20};
    int xpos[3] = { 300, 260, 220 };
    int z_pickup = -34;
    int drop_xyz[3] = {280, -100, 125};
    
    for(int x_travel = 0; x_travel < 3; x_travel++)
    {
		for(int y_travel = 0; y_travel < 2; y_travel++)
		{
			Dobot.goToXYZ(xpos[x_travel], ypos[y_travel], z_pickup, true);	//Go to the pickup position
			Dobot.setPump(true, true);										//Turn the pump on 
			//Dobot.wait(500);								
			//Dobot.goToXYZ(xpos[x_travel], drop_xyz[1], drop_xyz[2], true);		//Go to the ball drop location
			Dobot.goToXYZ(drop_xyz[0], drop_xyz[1], drop_xyz[2], true);		//Go to the ball drop location
			Dobot.setPump(false, true);										//Turn the pump off
			std::cout << "I'm trying to turn the pump off!" << std::endl;
			//Dobot.wait(500);
		}
	}
      
}   






