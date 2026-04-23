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
    //adc_init();
    // adc_gpio_init(26);
    // adc_gpio_init(27); //wire this


    //triangle
    float tri = 0.0;
    float step = 0.033;
    int direction = 1;
    //steps
    float t = 0;
    float dt = 0.005;

    
    while (true) {

        // //ramp waveform
        // for (int i=0; i<1024; i++){
        //     write_dac(0,i);
        //     sleep_ms(10);
        // }
        // sleep_ms(1000);

        // //triangle waveform
        // spi_write_tri();

        // //sine waveform
        // spi_write_sine();

        //sine wave on DAC A
        float voltage = (sin(2*M_PI*2*t)+1)*(3.3/2.0);
        write_DAC(0,voltage);
        //adc_select_input(0);
        //uint16_t rawA = adc_read();

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
        //adc_select_input(1);
        //uint16_t rawB = adc_read();

        //float voltA = rawA*3.3 / 4095.00;
        //float voltB = rawB*3.3 / 4095.00;
        sleep_ms(5);
        t += dt;

        //printf("sine wave voltage %.2f",voltA);
        //printf("tri wave voltage %.2f",voltB);
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
    uint16_t myV = v/3.3*1023;
    data[0] = 0b01110000;
    data[0] = data[0] | ((channel&0b1)<<7);
    data[0] = data[0] | (myV>>6)&0b00001111;

    data[1] = (myV<<2) & 0xFF;

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