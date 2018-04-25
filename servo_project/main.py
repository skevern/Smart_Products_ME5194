
from __future__ import division
import time
import Adafruit_PCA9685
from gpiozero import Button


# Initialize the PCA9685 using the default address (0x40).
pwm = Adafruit_PCA9685.PCA9685()

# Initialize the Microswitch
button = Button(5)

# Define our input and output objects
stamp_servo = 1
ink_servo = 0

# Configure min and max servo pulse lengths
ink_open = 470				#calibrated
ink_close = 220				#calibrated
stamp_rest = 450
stamp_paper = 375
stamp_ink = 300

# Helper function to make setting a servo pulse width simpler.
def set_servo_pulse(channel, pulse):
    pulse_length = 1000000    # 1,000,000 us per second
    pulse_length //= 60       # 60 Hz
    print('{0}us per period'.format(pulse_length))
    pulse_length //= 4096     # 12 bits of resolution
    print('{0}us per bit'.format(pulse_length))
    pulse *= 1000
    pulse //= pulse_length
    pwm.set_pwm(channel, 0, pulse)

# Set frequency to 60hz, good for servos.
pwm.set_pwm_freq(60)

def ink_stamp():
	for i in range(ink_close, ink_open):
		pwm.set_pwm(ink_servo, 0, i)			#Uncover the ink pad 
		time.sleep(0.005)
	time.sleep(1)
	pwm.set_pwm(stamp_servo, 0, stamp_ink)		#Move stamping arm to ink level
	time.sleep(2)
	pwm.set_pwm(stamp_servo, 0, stamp_rest)		#Move stamping arm to rest level
	time.sleep(1)
	for i in range(1, ink_open - ink_close):
		pwm.set_pwm(ink_servo, 0, ink_open - i)		#Cover the ink pad 
		time.sleep(0.005)
	
def paper_stamp():
	pwm.set_pwm(stamp_servo, 0, stamp_paper)
	time.sleep(2)
	pwm.set_pwm(stamp_servo, 0, stamp_rest)
	time.sleep(5)

#Ink the stamp upon starting the program
ink_stamp()

while True:
	# Ask the user when to stamp the paper
	# Replace this with continually reading the state of the microswitch
	input = not button.is_pressed
	print(input)
	
	if (input == False): 
		paper_stamp()
		ink_stamp()
		input = True;
	else:
		continue
	
