/**
 * 2021 www.redrobotics.co.uk - @NeilRedRobotics
 * Based on the PIO_PWM example code from Raspberry Pi
 * 
 * This code turns the Raspberry Pi Pico into a i2c secondary device.
 * For use with the Pico 2 Pi adaptor board and RedBoard+ motor driver.
 * 
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "pwm.pio.h"

#define I2C_PORT i2c0
#define I2C_SDA 20
#define I2C_SCL 21
#define I2C_ADDR 0x60// Can be set to: 0x61, 0x62 or 0x63


// Write `period` to the input shift register
void pio_pwm_set_period(PIO pio, uint sm, uint32_t period) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_put_blocking(pio, sm, period);
    pio_sm_exec(pio, sm, pio_encode_pull(false, false));
    pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
    pio_sm_set_enabled(pio, sm, true);
}

// Write `level` to TX FIFO. State machine will copy this into X.
void pio_pwm_set_level(PIO pio, uint sm, uint32_t level) {
    pio_sm_put_blocking(pio, sm, level);
}

int main() {
    stdio_init_all();

    // Set up output pins
    gpio_init(27); // Motor Dir A
    gpio_set_dir(27, GPIO_OUT);
    gpio_init(26); // Motor Dir B
    gpio_set_dir(26, GPIO_OUT);
    gpio_init(14); // Red Led
    gpio_set_dir(14, GPIO_OUT);
    gpio_init(17); // Green Led
    gpio_set_dir(17, GPIO_OUT);
    gpio_init(13); // Blue Led
    gpio_set_dir(13, GPIO_OUT);

    // Set up input pin
    gpio_init(7); // RedBoard button
    gpio_set_dir(7, GPIO_IN);
    gpio_pull_up(7);


    // Set up I2C
    i2c_init(I2C_PORT, 400*1000);
    i2c_set_slave_mode(i2c0, true, I2C_ADDR);

    gpio_set_function(I2C_SDA,GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL,GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);


    // Initialise PWM for servos
    gpio_set_function(2, GPIO_FUNC_PWM); // 1A
    gpio_set_function(3, GPIO_FUNC_PWM); // 1B
    gpio_set_function(4, GPIO_FUNC_PWM); // 2A
    gpio_set_function(5, GPIO_FUNC_PWM); // 2B

    gpio_set_function(8, GPIO_FUNC_PWM); // 4A
    gpio_set_function(9, GPIO_FUNC_PWM); // 4B
    gpio_set_function(10, GPIO_FUNC_PWM); // 5A
    gpio_set_function(11, GPIO_FUNC_PWM); // 5B

    gpio_set_function(12, GPIO_FUNC_PWM); // 6A Clash with GP28, use PIO for motor instead
    gpio_set_function(15, GPIO_FUNC_PWM); // 7B
    gpio_set_function(16, GPIO_FUNC_PWM); // 0A
    //gpio_set_function(19, GPIO_FUNC_PWM); // 1B clash with GP3 use PIO instead

    // Initialise PWM for motors
    gpio_set_function(22, GPIO_FUNC_PWM); // 3A
    // PIO is used for the other motor

    // Set Frequency
    pwm_set_wrap(0, 20000); // 50Hz for servos
    pwm_set_wrap(1, 20000); // 50Hz
    pwm_set_wrap(2, 20000); // 50Hz
    pwm_set_wrap(3, 62500); // 1KHz for motor
    pwm_set_wrap(4, 20000); // 50Hz
    pwm_set_wrap(5, 20000); // 50Hz
    pwm_set_wrap(6, 20000); // 50Hz
    pwm_set_wrap(7, 20000); // 50Hz

    // Set Clock Division
    pwm_set_clkdiv(0, 125.f); // 50Hz for servos
    pwm_set_clkdiv(1, 125.f);
    pwm_set_clkdiv(2, 125.f);
    pwm_set_clkdiv(3, 2.f); // 1KHz for motor
    pwm_set_clkdiv(4, 125.f);
    pwm_set_clkdiv(5, 125.f);
    pwm_set_clkdiv(6, 125.f);
    pwm_set_clkdiv(7, 125.f);

    // Enable PWM
    pwm_set_enabled(0, true);
    pwm_set_enabled(1, true);
    pwm_set_enabled(2, true);
    pwm_set_enabled(3, true);
    pwm_set_enabled(4, true);
    pwm_set_enabled(5, true);
    pwm_set_enabled(6, true);
    pwm_set_enabled(7, true);

    // Set Duty Cycle
    /*
    pwm_set_chan_level(0, PWM_CHAN_A,0); // GP16
    pwm_set_chan_level(1, PWM_CHAN_A,0); // GP2
    pwm_set_chan_level(1, PWM_CHAN_B,0); // GP3
    pwm_set_chan_level(2, PWM_CHAN_A,0); // GP4
    pwm_set_chan_level(2, PWM_CHAN_B,0); // GP5
    pwm_set_chan_level(3, PWM_CHAN_A,0); // GP22 Motor
    pwm_set_chan_level(4, PWM_CHAN_A,0); // GP8
    pwm_set_chan_level(4, PWM_CHAN_B,0); // GP9
    pwm_set_chan_level(5, PWM_CHAN_A,0); // GP10
    pwm_set_chan_level(5, PWM_CHAN_B,0); // GP11
    pwm_set_chan_level(6, PWM_CHAN_A,0); // GP12
    pwm_set_chan_level(7, PWM_CHAN_B,0); // GP15
    */

    

    // todo get free sm
    PIO pio = pio0;
    int sm0 = 0;
    int sm1 = 1;
    uint offset = pio_add_program(pio, &pwm_program);
    printf("Loaded program at %d\n", offset);

    // Set PIO PWM Pins & Frequency
    pwm_program_init(pio, sm0, offset, 28);
    pio_pwm_set_period(pio, sm0, 41650); // 1KHz for motor on GP28

    pwm_program_init(pio, sm1, offset, 19);
    pio_pwm_set_period(pio, sm1, 833000); // 50Hz for servo on GP19

    // Set PIO PWM duty cycle
    //pio_pwm_set_level(pio, sm0, 0); // Set motor duty cycle on GP28
    //pio_pwm_set_level(pio, sm1, 0); // Set servo duty cycle on GP19

    // Set up some variables
    uint8_t dataLen = 0;
    uint8_t i2cData[5];
    uint8_t i;
    float servoPulse;
    uint32_t sPulse;
    float s;

    sleep_ms(2000);
    printf("Start\n");

    while (true)
    {
        if(!gpio_get(7)) // Use the reset button on the RedBoard+ to toggle the Red LED
        {
            gpio_put(14,!gpio_get(14)); // Red LED
            sleep_ms(200);  
        }


        dataLen = (i2c_get_read_available(i2c0));
        if (dataLen != 0)
        {
            //printf("L=%d\n",dataLen);
            i2c_read_raw_blocking(i2c0, i2cData,4); // This seems to work better with a fixed data length

            // Serial printing is slow - only use for testing!
            /*
            printf("%d\n", i2cData[0]);
            printf("%d\n", i2cData[1]);
            printf("%d\n", i2cData[2]);
            printf("%d\n", i2cData[3]);
            printf("%d\n", i2cData[4]);
            printf("\n");
            */

            // LEDs
            if (i2cData[0] == 10)
            {
                switch (i2cData[1])
                {
                case 0:
                    gpio_put(14, i2cData[2]); // Red
                    break;
                
                case 1:
                    gpio_put(17, i2cData[2]); // Green
                    break;

                case 2:
                    gpio_put(13, i2cData[2]); // Blue
                    break;
                
                }
            }

            //Set PIO motor PWM
            else if (i2cData[0] == 30)
            {
                //printf("M0\n");
                //printf("Dir=%d\n",i2cData[1]);
                //printf("%d\n", i2cData[2]);
                gpio_put(27, i2cData[1]);
                pio_pwm_set_level(pio, sm0, i2cData[2]*163); // Set motor duty cycle on GP28

            } 

            // Set Motor PWM
            else if (i2cData[0] == 40)
            {
                //printf("M1\n");
                //printf("Dir=%d\n",i2cData[1]);
                //printf("%d\n", i2cData[2]);
                gpio_put(26, i2cData[1]);
                pwm_set_chan_level(3, PWM_CHAN_A,i2cData[2]*245); // Set motor duty cycle on GP22

            }           

            // Set Servos
            else if (i2cData[0] = 2)
            {
                // Convert the received data into one 16 bit number
                servoPulse = i2cData[2] << 8;
                servoPulse = servoPulse + i2cData[3];
                //printf("%d\n",i2cData[2]);
                //printf("%d\n",i2cData[3]);
                //printf("%d\n",servoPulse);
                
                switch (i2cData[1])
                {
                    case 0:
                        //printf("S19\n");
                        //printf("%d\n",servoPulse);
                        //printf("%.3f\n",s);
                        //printf("%d\n",sPulse);

                        //Scale up the pulse width to drive the servo with PIO 
                        s = (servoPulse-500)/2000;
                        sPulse = s * (104000-21000)+21000;
                        pio_pwm_set_level(pio, sm1, sPulse); // Set PWM on GP 19
                        break;

                    case 1:
                        pwm_set_chan_level(2, PWM_CHAN_B,servoPulse); // GP5
                        break;

                    case 2:
                        pwm_set_chan_level(2, PWM_CHAN_A,servoPulse); // GP4
                        break;

                    case 3:
                        pwm_set_chan_level(1, PWM_CHAN_B,servoPulse); // GP3 
                        break;   

                    case 4:
                        pwm_set_chan_level(1, PWM_CHAN_A,servoPulse); // GP2
                        break;

                    case 5:
                        pwm_set_chan_level(5, PWM_CHAN_A,servoPulse); // GP10    
                        break;

                    case 6:
                        pwm_set_chan_level(5, PWM_CHAN_B,servoPulse); // GP11
                        break;

                    case 7:
                        pwm_set_chan_level(6, PWM_CHAN_A,servoPulse); // GP12 
                        break;

                    case 8:
                        pwm_set_chan_level(4, PWM_CHAN_A,servoPulse); // GP8  
                        break;

                    case 9:
                        pwm_set_chan_level(0, PWM_CHAN_A,servoPulse); // GP16
                        break;

                    case 10:
                        pwm_set_chan_level(7, PWM_CHAN_B,servoPulse); // GP15  
                        break;

                    case 11:
                        pwm_set_chan_level(4, PWM_CHAN_B,servoPulse); // GP9    
                        break;
                }
            }   
        }

        // Clear I2C data
        for (i=0; i<5; i++)
        {
            i2cData[i] = 0;
        }

    }
}
