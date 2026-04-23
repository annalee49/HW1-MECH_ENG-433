#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_SPI_RX 16 //same as MISO
#define PIN_CS   17 //chip selection pin
#define PIN_SCK  18 //"clock"
#define PIN_SPI_TX 19 //same as MOSI
//LDAC is low, SHDN is high, Vrefs are both HIGH

//initializations for the functions
void write_DAC(int channel, float v);
static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);

//write DAC function that uses 
// void write_dac(uint8_t channel, uint16_t value)
// {
//     // limit to 10-bit input
//     value &= 0x03FF;

//     // scale to 12-bit for MCP4912
//     uint16_t dac_val = value << 2;

//     uint16_t command =
//         (channel << 15) |   // 0 = A, 1 = B
//         (1 << 14) |         // buffered
//         (1 << 13) |         // gain = 1x
//         (1 << 12) |         // active
//         (dac_val & 0x0FFF);

//     cs_select(PIN_CS);
//     spi_write16_blocking(SPI_PORT, &command, 1);
//     cs_deselect(PIN_CS);
// }


int main()
{
    stdio_init_all();

    // SPI initialisation, try at 12kHz first then 1000kHz
    spi_init(spi0, 1000 * 1000); // the baud, or bits per second, starting at 12kHz
    gpio_set_function(PIN_SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI_TX, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    //triangle
    float tri = 0.0;
    float step = 0.033;
    int direction = 1;
    //steps
    float t = 0;
    float dt = 0.01;

    
    while (true) {

        //sine wave on DAC A
        float voltage = (sin(2*M_PI*2*t)+1)*(3.3/2.0); //have to convert to radians, then add one and rescale
        float voltage = ((sin(2*M_PI*2*t)+1)/2)*3.3;
        write_DAC(0,voltage);

        //triangle wave on DAC B
        tri += direction * step;
        if (tri >= 3.3) {
            tri = 3.3;
            direction = -1;
        }
        if (tri <= 0.0) {
            tri = 0.0;
            direction = 1;
        }
        write_DAC(1,tri);

        sleep_ms(5);
        t += dt;
    }
}

void write_DAC(int channel, float v){
    uint8_t data[2];
    if (v<0){
        v=0;
    }
    if (v>3.3) {
        v=3.3;
    }
    uint16_t myV = v/3.3*1023; //0V to 1023 *Vref when gain setting is 1x
    //0b1111111111
    data[0] = 0b01110000; //write to DACa, buffered, 1x gain, active mode
    data[0] = data[0] | ((channel&0b1)<<7); //write to DACb or DACa
    data[0] = data[0] | (myV>>6)&0b00001111; //we only want the first 4 bits of this

    data[1] = (myV<<2) & 0xFF; //garbage bits on the ends (11,12)

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

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