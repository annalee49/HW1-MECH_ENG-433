#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_SPI_RX 16 //same as MISO
#define PIN_CS   17 //chip selection pin
#define PIN_SCK  18 //"clock"
#define PIN_SPI_TX 19 //same as MOSI

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);

void write_dac(uint8_t channel, uint16_t value)
{
    // limit to 10-bit input
    value &= 0x03FF;

    // scale to 12-bit for MCP4912
    uint16_t dac_val = value << 2;

    uint16_t command =
        (channel << 15) |   // 0 = A, 1 = B
        (1 << 14) |         // buffered
        (1 << 13) |         // gain = 1x
        (1 << 12) |         // active
        (dac_val & 0x0FFF);

    cs_select(PIN_CS);
    spi_write16_blocking(SPI_PORT, &command, 1);
    cs_deselect(PIN_CS);
}


int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c


    // SPI initialisation.
    spi_init(spi0, 12 * 1000); // the baud, or bits per second, starting at 12kHz
    gpio_set_function(PIN_SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI_TX, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    //initialize, dont touch cpol or cpha
    spi_set_format(SPI_PORT, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // send one DAC value
    uint16_t dac_val = 2048;

    //shift 0 to bit 15, 1 to bit 14, 1 to bit 13, 1 to bit 12
    uint16_t command =
    (0 << 15) |
    (1 << 14) |
    (1 << 13) |
    (1 << 12) |
    (dac_val & 0x0FFF);

    cs_select(PIN_CS);
    spi_write16_blocking(SPI_PORT, &command, 1);
    cs_deselect(PIN_CS);
    
    while (true) {
        for (int i=0; i<1024; i++){
            write_dac(0,i);
            sleep_ms(10);
        }
        sleep_ms(1000);
    }
}

//FUNCTIONS

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}


void write_spi(uint8_t data) {
    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, &data, 1);
    cs_deselect(PIN_CS);
}

;
