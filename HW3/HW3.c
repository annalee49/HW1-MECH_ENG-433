//DO NOT COMMIT THIS!! the current github works 

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
#define I2C_PORT i2c0 //default
#define I2C_SDA 8 //GP08 on Raspberry Pi Pico
#define I2C_SCL 9 //GP09 on Raspberry Pi Pico 
#define MCP23008_ADDR 0x20  //I2C address for MCP23008, A0, A1, A2 all grounded

// MCP23008 registers
#define IODIR_REG 0x00   //IODIR register address
#define OLAT_REG 0x0A    //OLAT register address
#define GPIO_REG 0x09    //GPIO register address (for reading input)

// Heartbeat LED pin (on Raspberry Pi Pico)
#define HEARTBEAT_PIN 15  //GP15 on the Raspberry Pi Pico

// Function to write a value to a specific register
void setPin(unsigned char address, unsigned char reg, unsigned char value) {
    uint8_t buf[2];
    buf[0] = reg;      //Register address
    buf[1] = value;    //Value to write
    i2c_write_blocking(I2C_PORT, address, buf, 2, false);
}

// Function to read a value from a specific register
unsigned char readPin(unsigned char address, unsigned char reg) {
    uint8_t buf[1];
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);  //Write register address to the device
    i2c_read_blocking(I2C_PORT, address, buf, 1, false);  //Read the data from the device
    return buf[0];  //Return the data read from the register
}

int main() {
    stdio_init_all();

    // Initialize I2C at 400 kHz
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    sleep_ms(2000); // Allow time for the system to initialize

    // Initialize heartbeat LED (GP15)
    gpio_init(HEARTBEAT_PIN);
    gpio_set_dir(HEARTBEAT_PIN, GPIO_OUT);

    // PART 1: initialize MCP23008 - GP7 as output, GP0 as input
    unsigned char iodir_value = 0x7F; //GP7 as output, GP0 as input, others as input
    setPin(MCP23008_ADDR, IODIR_REG, iodir_value); //set pin directions

    while (true) {
        // PART 2: blink heartbeat LED on GP15 (external LED)

        // Turn on the heartbeat LED
        gpio_put(HEARTBEAT_PIN, 1);
        sleep_ms(500);  //wait 500 ms

        // Turn off the heartbeat LED
        gpio_put(HEARTBEAT_PIN, 0);
        sleep_ms(500);  //wait 500 ms

        // PART 3: button and LED Logic (GP0 controls GP7)

        // Read the state of GP0 (button)
        unsigned char button_state = readPin(MCP23008_ADDR, GPIO_REG);

        // Check if GP0 (button) is pressed (LOW = pressed, HIGH = not pressed since pull up)
        if (button_state & 0x01) { //bit 0 (GP0) is HIGH, button not pressed
            //turn off GP7 LED (set low using OLAT)
            setPin(MCP23008_ADDR, OLAT_REG, 0x00);  //set GP7 LOW (binary 00000000)
            printf("GP7 LED OFF\n");
        } else {  //bit 0 (GP0) is LOW, button pressed
            //turn on GP7 LED (set high using OLAT)
            setPin(MCP23008_ADDR, OLAT_REG, 0x80);  //set GP7 HIGH (binary 10000000)
            printf("GP7 LED ON\n");
        }

        sleep_ms(100);  // Small delay to debounce button (for smoother operation)
    }

    return 0;
}