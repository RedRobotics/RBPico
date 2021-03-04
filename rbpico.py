# 04/03/2021 www.redrobotics.co.uk - @NeilRedRobotics
# Python Library to control a RedBoard+ via I2C using a
# Raspberry Pi PICO as an I2C secondary device.
# Up to four RedBoard+/Pico combinations can be connected
# together to control 8 DC motors and 48 servos.
# V0.0.1

import smbus
import time

Bus = smbus.SMBus(1)
PICO_ADDR = (0x60,0x61,0x62,0x63)

on = 1
off = 0

#Setup I2C
try:
    bus = smbus.SMBus(1)
except FileNotFoundError:
    print('')
    print('')
    print('I2C not enabled!')
    print('Enable I2C in raspi-config')

print("redboardpico Library v0.0.1 loaded")

# Check for connected PICO boards
count = 0
for i in range (4):

    try:
        bus.write_quick(PICO_ADDR[i])
        print("PICO RedBoard+ found on", hex(PICO_ADDR[i]))
    except OSError:
        count += 1

if count > 3:
    print ('No PICO boards found!')


# Servos 16 bit Pulse
def servo_P(pin,Pos):

    if pin <12:
        ADR = 0
    elif pin >11 and pin <24:
        ADR = 1
        pin = pin -12
    elif pin >23 and pin <36:
        ADR = 2
        pin = pin -24
    elif pin >35 and pin <48:
        ADR = 3
        pin = pin -36
    else:
        ADR = 4

    if (Pos >499 and Pos <2501) or Pos == 0:
        print ("servo",pin,"=",Pos)

        # Convert to 2x 8bit
        pos_h = Pos >> 8
        pos_l = Pos & 0xFF
        #print(pos_h)
        #print(pos_l)

        try:
            Bus.write_i2c_block_data(PICO_ADDR[ADR], 2, [pin, pos_h, pos_l])
        except OSError:
            print('No servo with that pin number!')
        except IndexError:
            print('Pin number too high!')
    else:
        print('Out of Range!')

# LEDs -------------------------------------------------------------------------
def r(state, board = 0):
    Bus.write_i2c_block_data(PICO_ADDR[board], 10, [0, state, 0])

def g(state, board = 0):
    Bus.write_i2c_block_data(PICO_ADDR[board], 10, [1, state, 0])

def b(state, board = 0):
    Bus.write_i2c_block_data(PICO_ADDR[board], 10, [2, state, 0])

def red(state = 0, board = 0):
    r(state, board)
    g(0, board)
    b(0, board)

def green(state = 0, board = 0):
    r(0, board)
    g(state, board)
    b(0, board)

def blue(state, board = 0):
    r(0, board)
    g(0, board)
    b(state, board)

def white(state, board = 0):
    r(state, board)
    g(state, board)
    b(state, board)

def cyan(state = 0, board = 0):
    r(0, board)
    g(state, board)
    b(state, board)

def yellow(state = 0, board = 0):
    r(state, board)
    g(state, board)
    b(0, board)

def magenta(state, board = 0):
    r(state, board)
    g(0, board)
    b(state, board)


# Motors -----------------------------------------------------------------------
def M_8bit(num,speed): # 0 - 255
    """Speed = -255 to +255"""

    if (num % 2) == 0: # Select motor 0 for even motor numbers
        m_num = int(num/2)
        motor = 30
    else:
        m_num = int((num-1)/2) # Select motor 1 for odd motor numbers
        motor = 40

    if speed > 255:  # Make sure the value sent to the motor is 255 or less
        print("Out of range")
        speed = 255

    elif speed < -255:  # Make sure the value sent to the motor is 255 or less
        print("Out of range")
        speed = -255

    if speed >= 0:  # Forwards
        print('Forwards')
        dir0 = 1

    elif speed < 0:  # Backwards
        print('Backwards')
        speed = abs(speed)  # Make positive
        dir0 = 0
        print (speed)

    try:
        print ('Motor',num,motor, 'Speed:',speed)
        bus.write_i2c_block_data(PICO_ADDR[m_num], motor, [dir0, speed,0])
    except OSError:
        print('No motor with that pin number!')
    except IndexError:
        print("PICO Not Available On That Address!")

# ADC --------------------------------------------------------------------------
def readAdc_0():
    bus.write_i2c_block_data(0x48, 0x01, [0xc3, 0x83])
    time.sleep(0.01)
    voltage0 = bus.read_i2c_block_data(0x48,0x00,2)
    conversion_0 = (voltage0[1])+(voltage0[0]<<8)
    volts_0 = conversion_0 / ADC_bat_conversion_Value
    return (round(volts_0,2))


def readAdc_1():
    bus.write_i2c_block_data(0x48, 0x01, [0xd3, 0x83])
    time.sleep(0.1)
    voltage1 = bus.read_i2c_block_data(0x48,0x00,2)
    conversion_1 = (voltage1[1])+(voltage1[0]<<8)
    volts_1 = conversion_1 / 7891 # 3.3 Volts
    return round(volts_1,2)


def readAdc_2():
    bus.write_i2c_block_data(0x48, 0x01, [0xe3, 0x83])
    time.sleep(0.1)
    voltage2 = bus.read_i2c_block_data(0x48,0x00,2)
    conversion_2 = (voltage2[1])+(voltage2[0]<<8)
    volts_2 = conversion_2 / 7891 # 3.3 Volts
    return round(volts_2,2)


def readAdc_3():
    bus.write_i2c_block_data(0x48, 0x01, [0xf3, 0x83])
    time.sleep(0.1)
    voltage3 = bus.read_i2c_block_data(0x48,0x00,2)
    conversion_3 = (voltage3[1])+(voltage3[0]<<8)
    volts_3 = conversion_3 / 7891 # 3.3 Volts
    return round(volts_3,2)

#----------------------------------------------------------







'''
while (True):
    Bus.write_i2c_block_data(ADDR, 10, [0, 64,0,0])
    time.sleep(0.1)
    Bus.write_i2c_block_data(ADDR, 20, [0, 64,0,0])
    time.sleep(2)

    Bus.write_i2c_block_data(ADDR, 10, [0, 250,0,0])
    time.sleep(0.1)
    Bus.write_i2c_block_data(ADDR, 20, [0, 250,0,0])
    time.sleep(2)
'''
