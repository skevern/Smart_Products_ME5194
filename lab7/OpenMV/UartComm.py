#Collect Keypoints for Calibration By Dylan DeSantis
##################################
# This program demonstrates how to communicate over uart
# with the raspberry pi.
##################################

import sensor, image, time
from pyb import UART
#Sensor Setup
#################################
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
#################################
def Uart_sender:
    #Communication Setup
    #################################
    uart = UART(1, 9600, timeout_char=1000)                         # init with given baudrate
    uart.init(9600, bits=8, parity=None, stop=1, timeout_char=1000) # init with given parameters
    #################################

    #Main Loop
    #################################
    clock = time.clock()
    while(True):
        clock.tick()
        while(uart.any() ==0):
            continue
        #print(20)
        while(uart.any() !=0):
            print("The returned value is: " ,uart.readchar())
        time.sleep(1000)
        uart.writechar(8)
    #################################
