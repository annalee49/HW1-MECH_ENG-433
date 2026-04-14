#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
#define I2C_PORT i2c0 //default
#define I2C_SDA 8 //GP08 on Raspberry Pi Pico
#define I2C_SCL 9 //GP09 on Raspberry Pi Pico 

// Heartbeat LED pin (on Raspberry Pi Pico)
#define HEARTBEAT_PIN 15  //GP15 on the Raspberry Pi Pico

int main() {
    stdio_init_all();

    //initialize I2C at 400 kHz
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);


    sleep_ms(2000); //allow time for the system to initialize

    //initialize heartbeat LED (GP15)
    gpio_init(HEARTBEAT_PIN);
    gpio_set_dir(HEARTBEAT_PIN, GPIO_OUT);

    //initialize SSD1306 OLED display
    ssd1306_setup();

    //clear the display buffer initially
    ssd1306_clear();

    bool pixel_on = false;

    while (true) {

        //heartbeat for the pico
        
        //turn on the heartbeat LED
        gpio_put(HEARTBEAT_PIN, 1);
        sleep_ms(500);  //wait 500 ms

        //turn off the heartbeat LED
        gpio_put(HEARTBEAT_PIN, 0);
        sleep_ms(500);  //wait 500 ms

        //toggle pixel state at 1 Hz (every second)
        pixel_on = !pixel_on;

        //using pixel 0,0 to flash at 1 Hz
        ssd1306_drawPixel(0, 0, pixel_on ? 1 : 0);

        //update the OLED display to show changes
        ssd1306_update();


        //drawChar(10, 10, 1, 'A');  // Draw 'A' at (10,10) in white
        //ssd1306_update();          // Push to display

    }

    return 0;
}