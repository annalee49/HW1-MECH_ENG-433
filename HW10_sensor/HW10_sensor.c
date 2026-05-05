#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
#define LEFT_PIN 14
#define JUMP_PIN 15
#define RIGHT_PIN 16

int main() {
    stdio_init_all();  // enables USB serial

    gpio_init(JUMP_PIN);
    gpio_set_dir(JUMP_PIN, GPIO_IN);
    gpio_disable_pulls(JUMP_PIN);

    gpio_init(RIGHT_PIN);
    gpio_set_dir(RIGHT_PIN, GPIO_IN);
    gpio_disable_pulls(RIGHT_PIN);
    
    gpio_init(LEFT_PIN);
    gpio_set_dir(LEFT_PIN, GPIO_IN);
    gpio_disable_pulls(LEFT_PIN);

    int last_left = 1;
    int last_jump = 1;
    int last_right = 1;

    while (true) {
    printf("L:%d J:%d R:%d\n",
        gpio_get(LEFT_PIN),
        gpio_get(JUMP_PIN),
        gpio_get(RIGHT_PIN)
    );
    sleep_ms(20);
    }
}