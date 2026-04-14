#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"


// I2C defines
#define I2C_PORT i2c0 //default
#define I2C_SDA 8 //GP08 on Raspberry Pi Pico
#define I2C_SCL 9 //GP09 on Raspberry Pi Pico 
#define MCP23008_ADDR 0x20  //I2C address for MCP23008, A0, A1, A2 all grounded

// Heartbeat LED pin (on Raspberry Pi Pico)
#define HEARTBEAT_PIN 15  //GP15 on the Raspberry Pi Pico

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

    // Initialize SSD1306 OLED display
    ssd1306_setup();

    // Clear the display buffer initially
    ssd1306_clear();

    bool pixel_on = false;

    while (true) {
        // PART 2: blink heartbeat LED on GP15 (external LED)

        // Turn on the heartbeat LED
        gpio_put(HEARTBEAT_PIN, 1);
        sleep_ms(500);  //wait 500 ms

        // Turn off the heartbeat LED
        gpio_put(HEARTBEAT_PIN, 0);
        sleep_ms(500);  //wait 500 ms

        // Toggle pixel state at 1 Hz (every second)
        pixel_on = !pixel_on;

        // using pixel 0,0 to flash at 1 Hz
        ssd1306_drawPixel(0, 0, pixel_on ? 1 : 0);

    }

    return 0;
}