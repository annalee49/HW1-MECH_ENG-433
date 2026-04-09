#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/cyw43_arch.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0 //correct for this raspberry pi
#define I2C_SDA 8 //GP08 on the raspberry pi
#define I2C_SCL 9 //GP09 on the raspberry pi 
#define i2c_default PICO_DEFAULT_I2C_INSTANCE()
#define MCP23008_ADDR 0x20 //A0,A1,A2 are all grounded
#define heartbeat_pin 15 //GP15 on the raspberry pi

int main()
{
    stdio_init_all();
    cyw43_arch_init();

    sleep_ms(5000); // <-- IMPORTANT

    // I2C Initialisation. Using it at 400Khz.
    printf("Starting I2C init\n");
    i2c_init(i2c0, 400*1000);
    printf("I2C init done\n");
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    printf("set up SDA and SCL\n");
    //gpio_pull_up(I2C_SDA); using my own external
    //gpio_pull_up(I2C_SCL); using my own external
    //GP7 should be output and GP0 should be input (chip not the raspberry pi)
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // Example to turn on the Pico W LED
    //cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    //PART 1: initialize the I2C
    uint8_t buf[2];
    buf[0] = 0x00;   //IODIR register address
    buf[1] = 0x7F;   //set directions GP7 output, GP0 input, others input

    i2c_write_blocking(i2c0, MCP23008_ADDR, buf, 2, false); 
    printf("initialization successful\n");

    // while (true) {
    //     printf("Hello, USB!\n");
    //     sleep_ms(1000);
    //     printf("save us save us\n");
    //     sleep_ms(1000);
    // }
    
    //PART 2: heartbeat LED

    gpio_init(heartbeat_pin);
    gpio_set_dir(heartbeat_pin, GPIO_OUT);

    while (true) {
        gpio_put(heartbeat_pin, 1);
        sleep_ms(500);

        gpio_put(heartbeat_pin, 0);
        sleep_ms(500);

        //i currently have my heartbeat light (blue) connected to GP15 on the raspberry pi
        //try to blink LED that i add to my board, blink GP7
        //once that works, try to read from GP0 and then blink GP7

        //PART 3: getting the LED on GP7 to blink

        //turn on GP7 led (set high)
        buf[0] = 0x0A;        // OLAT register address
        buf[1] = 0x80;  // set only GP7 to HIGH
        i2c_write_blocking(i2c0, MCP23008_ADDR, buf, 2, false);
        printf("GP7 LED ON\n");
        sleep_ms(500);  // wait for 500ms

        // turn off GP7 (set low)
        buf[1] = 0x00;      // set all pins to LOW
        i2c_write_blocking(i2c0, MCP23008_ADDR, buf, 2, false);
        printf("GP7 LED OFF\n");
        sleep_ms(500);  // wait for 500ms
    }
   

//ADDR is 0x00, 0x01, 0x02...
//buf is the array of 8 bit data
//2 or 1 is the length of the array 
//false or true 

//sending (aka writing) data
//i2c_write_blocking(i2c0, ADDR, buf, 2, false);

//reading data
//i2c_write_blocking(i2c0, ADDR, &reg, 1, true);  // true to keep host control of bus
//i2c_read_blocking(i2c0, ADDR, &buf, 1, false);  // false - finished with bus

}