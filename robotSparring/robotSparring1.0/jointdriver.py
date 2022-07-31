#https://github.com/adafruit/Adafruit_Python_PCA9685
#sudo pip install adafruit-pca9685

from __future__ import division
import time
import Adafruit_PCA9685.PCA9685

# Uncomment to enable debug output.
#import logging
#logging.basicConfig(level=logging.DEBUG)

#for i2c list use:
#i2cdetect -y 1 
#Initialise the PCA9685 using the default address (0x40).
pwm = Adafruit_PCA9685.PCA9685()
# Alternatively specify a different address and/or bus:
#pwm = Adafruit_PCA9685.PCA9685(address=0x41, busnum=2)

# Configure min and max servo pulse lengths
servo_min = 150  # Min pulse length out of 4096
servo_max = 500  # Max pulse length out of 4096

# Helper function to make setting a servo pulse width simpler.
def set_servo_pulse(channel, pulse):
    pulse_length = 1000000    # 1,000,000 us per second
    pulse_length //= 50       # 60 Hz
    print('{0}us per period'.format(pulse_length))
    pulse_length //= 4096     # 12 bits of resolution
    print('{0}us per bit'.format(pulse_length))
    pulse *= 1000
    pulse //= pulse_length
    pwm.set_pwm(channel, 0, pulse)

#get pulse from degree
def map(x):
    degree_min = -90
    degree_max = 90
    pulse_min = 150
    pulse_max = 600
    y = (x - degree_min) * (pulse_max - pulse_min) / (degree_max - degree_min) + pulse_min
    return int(y)

# Set frequency to 60hz, good for servos.
pwm.set_pwm_freq(50)

def headScanning(delay):
    for degree in [-30, 20, 0, -20, 30]:
        pwm.set_pwm(0, 0, map(degree))
        for degree2 in [0, -20, 20, -10, 10, 0]:
            pwm.set_pwm(1, 0, map(degree2))
            time.sleep(delay)
			
    for degree in [-40, 30, -20, 10, 0, 40, -30, 20, -10, 0]:
        pwm.set_pwm(0, 0, map(degree))
        time.sleep(delay)
        pwm.set_pwm(1, 0, map(degree))
        time.sleep(delay)

#body joint
HEAD = 0
NECK = 1
RIGHT_FRONT_SHOULDER=2
RIGHT_FRONT_ARM=3
LEFT_FRONT_SHOULDER=4
LEFT_FRONT_ARM=5
RIGHT_BACK_SHOULDER=6
RIGHT_BACK_ARM=7
LEFT_BACK_SHOULDER=8
LEFT_BACK_ARM=9

#moveJoint
def moveJoint(joint, degree):
    sign = 1
    if(joint in [NECK, LEFT_FRONT_ARM, RIGHT_BACK_ARM, RIGHT_FRONT_SHOULDER, RIGHT_BACK_SHOULDER]):
        sign = -1
    
    pwm.set_pwm(joint, 0, map(degree*sign))
   
    #debug della posizione
    #print("pwm.set_pwm("+str(joint)+", 0, map("+str(degree*sign)+"))")

#zero pos
def zero(arm_zero_pos=0):
    moveJoint(HEAD, 0)
    moveJoint(NECK, 0)
    moveJoint(RIGHT_FRONT_SHOULDER, 0)
    moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
    moveJoint(LEFT_FRONT_SHOULDER, 0)
    moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
    moveJoint(RIGHT_BACK_SHOULDER, 0)
    moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
    moveJoint(LEFT_BACK_SHOULDER, 0)
    moveJoint(LEFT_BACK_ARM, arm_zero_pos)

#stendup
def standup():
    for degree in [50]:
        moveJoint(HEAD, 0)
        moveJoint(NECK, 0)
        moveJoint(RIGHT_FRONT_SHOULDER, 0)
        moveJoint(RIGHT_FRONT_ARM, degree)
        moveJoint(LEFT_FRONT_SHOULDER, 0)
        moveJoint(LEFT_FRONT_ARM, degree)
        moveJoint(RIGHT_BACK_SHOULDER, 0)
        moveJoint(RIGHT_BACK_ARM, degree)
        moveJoint(LEFT_BACK_SHOULDER, 0)
        moveJoint(LEFT_BACK_ARM, degree)

#relax motors
def relax():
    zero()
    time.sleep(0.1)
    pwm = Adafruit_PCA9685.PCA9685()

#step forward
def stepForward(stepCount=0, delay=0.1, arm_zero_pos=50):
    zero(arm_zero_pos)
    time.sleep(delay)
    #conta i passi in avanti
    for i in range(stepCount):
        #porta il peso indietro a sinistra
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos-50)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos-70)
        time.sleep(delay)
        #muove a gamba anteriore destra per il passo
        moveJoint(RIGHT_FRONT_ARM, -40)
        time.sleep(delay)
        moveJoint(RIGHT_FRONT_SHOULDER, 40)
        #moveJoint(NECK, 40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        #moveJoint(NECK, -40)
        time.sleep(delay)
        #porta il peso in avanti a destra
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos-90)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos-20)
        time.sleep(delay)
        #muove a gamba posteriore sinistra per il passo
        moveJoint(LEFT_BACK_ARM, -40)
        time.sleep(delay)
        moveJoint(LEFT_BACK_SHOULDER, 40)
        #moveJoint(NECK, 0)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        #moveJoint(NECK, -30)
        time.sleep(delay)
        #effettua la spinta per il passo
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_SHOULDER, 0)
        moveJoint(LEFT_FRONT_SHOULDER, -40)
        moveJoint(LEFT_BACK_SHOULDER, 0)
        moveJoint(RIGHT_BACK_SHOULDER, -40)
        #moveJoint(NECK, 40)
        time.sleep(delay)

        #porta il peso indietro a destra
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos-70)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos-50)
        time.sleep(delay)
        #muove a gamba anteriore sinistra per il passo
        moveJoint(LEFT_FRONT_ARM, -40)
        time.sleep(delay)
        moveJoint(LEFT_FRONT_SHOULDER, 40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        time.sleep(delay)
        #porta il peso in avanti a sinistra
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos-50)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos-70)
        time.sleep(delay)
        #muove a gamba posteriore destra per il passo
        moveJoint(RIGHT_BACK_ARM, -40)
        time.sleep(delay)
        moveJoint(RIGHT_BACK_SHOULDER, 40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        time.sleep(delay)
        #effettua la spinta per il passo
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_SHOULDER, -40)
        moveJoint(LEFT_FRONT_SHOULDER, 0)
        moveJoint(LEFT_BACK_SHOULDER, -40)
        moveJoint(RIGHT_BACK_SHOULDER, 0)
        time.sleep(delay)

#step back
def stepBack(stepCount=0, delay=0.1, arm_zero_pos=50):
    zero(arm_zero_pos)
    time.sleep(delay)
    #conta i passi in avanti
    for i in range(stepCount):
        #porta il peso in avanti a sinistra
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos-50)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos-70)
        time.sleep(delay)
        #muove a gamba posteriore destra per il passo
        moveJoint(RIGHT_BACK_ARM, -40)
        time.sleep(delay)
        moveJoint(RIGHT_BACK_SHOULDER, -40)
        #moveJoint(NECK, 40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        #moveJoint(NECK, -40)
        time.sleep(delay)
        #porta il peso indietro a destra
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos-90)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos-20)
        time.sleep(delay)
        #muove a gamba anteriore sinistra per il passo
        moveJoint(LEFT_FRONT_ARM, -40)
        time.sleep(delay)
        moveJoint(LEFT_FRONT_SHOULDER, -40)
        #moveJoint(NECK, 0)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        #moveJoint(NECK, -30)
        time.sleep(delay)
        #effettua la spinta per il passo
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_SHOULDER, 0)
        moveJoint(LEFT_BACK_SHOULDER, 40)
        moveJoint(LEFT_FRONT_SHOULDER, 0)
        moveJoint(RIGHT_FRONT_SHOULDER, 40)
        #moveJoint(NECK, 40)
        time.sleep(delay)

        #porta il peso in avanti a destra
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos-70)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos-50)
        time.sleep(delay)
        #muove a gamba posteriore sinistra per il passo
        moveJoint(LEFT_BACK_ARM, -40)
        time.sleep(delay)
        moveJoint(LEFT_BACK_SHOULDER, -40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        time.sleep(delay)
        #porta il peso indietro a sinistra
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos-50)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos-70)
        time.sleep(delay)
        #muove a gamba anteriore destra per il passo
        moveJoint(RIGHT_FRONT_ARM, -40)
        time.sleep(delay)
        moveJoint(RIGHT_FRONT_SHOULDER, -40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        time.sleep(delay)
        #effettua la spinta per il passo
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_SHOULDER, 40)
        moveJoint(LEFT_BACK_SHOULDER, 0)
        moveJoint(LEFT_FRONT_SHOULDER, 40)
        moveJoint(RIGHT_FRONT_SHOULDER, 0)
        time.sleep(delay)

#step turn left
def stepTurnLeft(stepCount=0, delay=0.1, arm_zero_pos=50):
    zero(arm_zero_pos)
    time.sleep(delay)
    #conta i passi in avanti
    for i in range(stepCount):
        #porta il peso indietro a sinistra
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos-50)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos-70)
        time.sleep(delay)
        #muove a gamba anteriore destra per il passo
        moveJoint(RIGHT_FRONT_ARM, -40)
        time.sleep(delay)
        moveJoint(RIGHT_FRONT_SHOULDER, 40)
        #moveJoint(NECK, 40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        #moveJoint(NECK, -40)
        time.sleep(delay)
        #porta il peso in avanti a destra
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos-90)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos-20)
        time.sleep(delay)
        #muove a gamba posteriore sinistra per il passo
        moveJoint(LEFT_BACK_ARM, -40)
        time.sleep(delay)
        moveJoint(LEFT_BACK_SHOULDER, -40)
        #moveJoint(NECK, 0)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        #moveJoint(NECK, -30)
        time.sleep(delay)
        #effettua la spinta per il passo
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_SHOULDER, 0)
        moveJoint(LEFT_FRONT_SHOULDER, 40)
        moveJoint(LEFT_BACK_SHOULDER, 0)
        moveJoint(RIGHT_BACK_SHOULDER, -40)
        #moveJoint(NECK, 40)
        time.sleep(delay)

        #porta il peso indietro a destra
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos-70)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos-50)
        time.sleep(delay)
        #muove a gamba anteriore sinistra per il passo
        moveJoint(LEFT_FRONT_ARM, -40)
        time.sleep(delay)
        moveJoint(LEFT_FRONT_SHOULDER, -40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        time.sleep(delay)
        #porta il peso in avanti a sinistra
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos-50)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos-70)
        time.sleep(delay)
        #muove a gamba posteriore destra per il passo
        moveJoint(RIGHT_BACK_ARM, -40)
        time.sleep(delay)
        moveJoint(RIGHT_BACK_SHOULDER, 40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        time.sleep(delay)
        #effettua la spinta per il passo
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_SHOULDER, -40)
        moveJoint(LEFT_FRONT_SHOULDER, 0)
        moveJoint(LEFT_BACK_SHOULDER, 40)
        moveJoint(RIGHT_BACK_SHOULDER, 0)
        time.sleep(delay)

#step turn right
def stepTurnRight(stepCount=0, delay=0.1, arm_zero_pos=50):
    zero(arm_zero_pos)
    time.sleep(delay)
    #conta i passi in avanti
    for i in range(stepCount):
        #porta il peso indietro a sinistra
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos-50)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos-70)
        time.sleep(delay)
        #muove a gamba anteriore destra per il passo
        moveJoint(RIGHT_FRONT_ARM, -40)
        time.sleep(delay)
        moveJoint(RIGHT_FRONT_SHOULDER, -40)
        #moveJoint(NECK, 40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        #moveJoint(NECK, -40)
        time.sleep(delay)
        #porta il peso in avanti a destra
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos-90)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos-20)
        time.sleep(delay)
        #muove a gamba posteriore sinistra per il passo
        moveJoint(LEFT_BACK_ARM, -40)
        time.sleep(delay)
        moveJoint(LEFT_BACK_SHOULDER, 40)
        #moveJoint(NECK, 0)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        #moveJoint(NECK, -30)
        time.sleep(delay)
        #effettua la spinta per il passo
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_SHOULDER, 0)
        moveJoint(LEFT_FRONT_SHOULDER, -40)
        moveJoint(LEFT_BACK_SHOULDER, 0)
        moveJoint(RIGHT_BACK_SHOULDER, 40)
        #moveJoint(NECK, 40)
        time.sleep(delay)

        #porta il peso indietro a destra
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos-70)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos-50)
        time.sleep(delay)
        #muove a gamba anteriore sinistra per il passo
        moveJoint(LEFT_FRONT_ARM, -40)
        time.sleep(delay)
        moveJoint(LEFT_FRONT_SHOULDER, 40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        time.sleep(delay)
        #porta il peso in avanti a sinistra
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos-50)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos-70)
        time.sleep(delay)
        #muove a gamba posteriore destra per il passo
        moveJoint(RIGHT_BACK_ARM, -40)
        time.sleep(delay)
        moveJoint(RIGHT_BACK_SHOULDER, -40)
        time.sleep(delay)
        #reimposta l'assetto gambe
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        time.sleep(delay)
        #effettua la spinta per il passo
        moveJoint(RIGHT_FRONT_ARM, arm_zero_pos)
        moveJoint(LEFT_FRONT_ARM, arm_zero_pos)
        moveJoint(RIGHT_BACK_ARM, arm_zero_pos)
        moveJoint(LEFT_BACK_ARM, arm_zero_pos)
        moveJoint(RIGHT_FRONT_SHOULDER, 40)
        moveJoint(LEFT_FRONT_SHOULDER, 0)
        moveJoint(LEFT_BACK_SHOULDER, -40)
        moveJoint(RIGHT_BACK_SHOULDER, 0)
        time.sleep(delay)
