#OpenMV Control & Communication with RPI
#by: Scott Kevern
##################################
# This program detects the circles in an image and
# outputs their colors, sizes, and locations to the RPI
# via a UART communication channel
##################################
import sensor, image, time, math
from image import *
from pyb import UART
import math

#Sensor Setup
#################################
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
#sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_contrast(3)
sensor.set_brightness(-3)
sensor.set_gainceiling(16)
#sensor.set_framesize(sensor.VGA)
sensor.set_framesize(sensor.QQVGA)
#sensor.set_windowing((480,400))
#sensor.set_lens_correction(True,25,500)
sensor.skip_frames(time = 4000)
sensor.set_auto_gain(False, value =100)
clock = time.clock()
#################################

#Communication Setup
#################################
uart = UART(1, 9600, timeout_char=1000)                         # init with given baudrate
uart.init(9600, bits=8, parity=None, stop=1, timeout_char=1000) # init with given parameters

def Uart_sender():
    clock = time.clock()
    loop_control = True
    while(loop_control):
        clock.tick()

        #Wait while there is no data available
        while(uart.any() == 0):
            pass
        #Now that there is data available, print it!
        while(True):
            returned_value = uart.readchar()
            if (returned_value == 13):
                #Send a response until it is received and confirmed
                read_keys = 600
                uart.writechar(read_keys)
                print("I wrote back the value: ", read_keys)
                break

#################################

#Ball Identification Function
#################################
def Ball_Identifier():
    blue_t = (40, 100, 140)
    blue_t = (90, 90, 110)
    pink_t = (180, 155, 155)
    green_t = (70, 160, 90)
    yellow_t = (185, 150, 80)
    orange_t = (240, 140, 70)
    red_t = (160,130,120)
    circle_data = []
    window_margin = 50;
    target_center_x = 72;
    target_center_y = 55;
    img = sensor.snapshot()#.lens_corr(1.7)#.morph(1,[2,4,-2,-4,2,4,-2,-4,2],mult=10,add=0).gaussian(3)
    color = 0
    for c in img.find_circles(threshold =2000 , x_margin = 10, y_margin = 10, r_margin = 10):
        #Filter by color
        stats = img.get_statistics(roi=(c[0], c[1], c[2], c[3]))
        rgb_t = image.lab_to_rgb((stats.l_mean(), stats.a_mean(), stats.b_mean()))
        #Draw selection window
        img.draw_rectangle([72-int(window_margin/2), target_center_y - int(window_margin/2), window_margin, window_margin])
        if (abs(72 - c.x()) < window_margin/2) and (abs(55-c.y()) < window_margin/2):
            if math.fabs(rgb_t[0] - blue_t[0]) < 20 and math.fabs(rgb_t[1] - blue_t[1]) < 20 and math.fabs(rgb_t[2] - blue_t[2]) < 20:
                circle_data.append(c[0]);
                circle_data.append(c[1]);
                circle_data.append(c[2]);
                circle_data.append(c[3]);
                print("Found a blue ball!")
                img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                color = 1

            if math.fabs(rgb_t[0] - pink_t[0]) < 20 and math.fabs(rgb_t[1] - pink_t[1]) < 20 and math.fabs(rgb_t[2] - pink_t[2]) < 20:
                circle_data.append(c[0]);
                circle_data.append(c[1]);
                circle_data.append(c[2]);
                circle_data.append(c[3]);
                print("Found a pink ball!")
                img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                color = 2

            if math.fabs(rgb_t[0] - green_t[0]) < 20 and math.fabs(rgb_t[1] - green_t[1]) < 20 and math.fabs(rgb_t[2] - green_t[2]) < 20:
                circle_data.append(c[0]);
                circle_data.append(c[1]);
                circle_data.append(c[2]);
                circle_data.append(c[3]);
                print("Found a green ball!")
                img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                color = 3

            if math.fabs(rgb_t[0] - yellow_t[0]) < 20 and math.fabs(rgb_t[1] - yellow_t[1]) < 20 and math.fabs(rgb_t[2] - yellow_t[2]) < 20:
                circle_data.append(c[0]);
                circle_data.append(c[1]);
                circle_data.append(c[2]);
                circle_data.append(c[3]);
                print("Found a yellow ball!")
                img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                color = 4

            if math.fabs(rgb_t[0] - orange_t[0]) < 20 and math.fabs(rgb_t[1] - orange_t[1]) < 20 and math.fabs(rgb_t[2] - orange_t[2]) < 20:
                circle_data.append(c[0]);
                circle_data.append(c[1]);
                circle_data.append(c[2]);
                circle_data.append(c[3]);
                print("Found an orange ball!")
                img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                color = 5
            if math.fabs(rgb_t[0] - red_t[0]) < 20 and math.fabs(rgb_t[1] - red_t[1]) < 20 and math.fabs(rgb_t[2] - red_t[2]) < 20:
                circle_data.append(c[0]);
                circle_data.append(c[1]);
                circle_data.append(c[2]);
                circle_data.append(c[3]);
                print("Found a red ball!")
                img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                color = 6
                array = [c.x(), c.y(), c.r(), color]
                print(array)
                print(rgb_t)

            if (color != 0):
                #img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                array = [c.x(), c.y(), c.r(), color, 0]
                listmanager(array)
            img.draw_circle(72,55,15, color = (255, 255, 255))
            #print(rgb_t, c.r())
#################################

#List  Management Function
#################################
mylist = []
def listmanager(input_array):
    err_total = 15
    thresh = 3
    for iter_array in mylist:
        errx = abs(iter_array[0] - input_array[0])
        erry = abs(iter_array[1] - input_array[1])
        err_temp= math.sqrt(errx^2 + erry^2)
        if (err_temp < err_total):
            iter_array[4] += 1
    print(err_total)
    if (err_total > thresh):
       mylist.append(input_array)
       print(input_array)

#Main Loop
#################################
while(True):
    clock.tick()
    for i in range(0,30):
        Ball_Identifier()
        time.sleep(1)
    Uart_sender()
    print("Uart_sender() function is over")
    time.sleep(100000)
#################################


