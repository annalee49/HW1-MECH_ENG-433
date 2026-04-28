#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include <math.h>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS_DAC  17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_CS_RAM 13 //CHANGE THIS WHEN YOU ACTUALLY WIRE IT 
//mosi is serial data in, miso is serial data out
//mosi is tx, miso is rx

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);

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

void update_dac(uint8_t channel, float voltage);
void update_dac_from_ram(int);

void spi_ram_init();
void spi_ram_write(uint16_t, uint8_t *, int);
void spi_ram_read(uint16_t, uint8_t *, int);

void ram_write_sine();

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_DAC,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_CS_RAM,   GPIO_FUNC_SIO);

    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_put(PIN_CS_DAC, 1);

    gpio_set_dir(PIN_CS_RAM, GPIO_OUT);
    gpio_put(PIN_CS_RAM, 1);

    spi_ram_init();
    ram_write_sine();
    int i =0;

    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    while (true) {

        // for (i=0; i<1024; i++){
        //     update_dac(0, (float)i*3.3/1024);
        //     sleep_ms(1);
        // }

        for (i=0; i<1024*2; i=i+2){
            update_dac_from_ram(i);
            sleep_ms(1);
        }
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

    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS_DAC);
}

void update_dac(uint8_t channel, float voltage){
    uint8_t data[2];

    uint16_t data_short = 0;
    data_short = (channel&0b1)<<15;
    data_short = data_short | (0b111<<12);

    uint16_t v = voltage / 3.3 *1024;
    data_short = data_short | (v << 2);
    data[0] = data_short >>8;
    data[1] = data_short &0xFF;

    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, data, 2);
    cs_deselect(PIN_CS_DAC);
}

void update_dac_from_ram(int i){
    uint8_t data[2];
    spi_ram_read(i, data, 2); //read from the address and put the bytes you are reading from into the array

    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, data, 2); //send the array right back
    cs_deselect(PIN_CS_DAC);
}

void ram_write_sine(){
    int i=0;
    uint8_t data[2];
    uint16_t data_short = 0;
    uint8_t channel = 0b0;
    float voltage = 0;
    uint16_t addr=0;
    
    for (i=0; i<1024; i++){
        data_short = (channel&0b1) <<15;
        data_short = data_short | (0b111<<12);

        //voltage = (sin(2*M_PI*i/1024.0)+1)*(3.3/2.0);
        voltage = (sin(2*M_PI*i/1024.0)+1)*512;

        uint16_t v = voltage;
        data_short = data_short | (0b111111111111 & v);

        data[0] = data_short >> 8;
        data[1] = data_short & 0xFF;

        spi_ram_write(addr, data, 2);
        addr = addr + 2;
    }
}

void spi_ram_init(){
    uint8_t data[2];
    int len = 2;
    data[0] = 0b00000001; //from datasheet, let me write to you
    data[1] = 0b01000000; //from datasheet, sequential mode
    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, data, len);
    cs_deselect(PIN_CS_RAM);
}

void spi_ram_write(uint16_t addr, uint8_t * data, int len){
    uint8_t packet[5]; //addr = 0b1111111111111111, we are trying to send 8 at a time, with a >> shift, the high second 8 are kept, and then its 0b00000000111111111
    packet[0] = 0b00000010; //instruction, write
    packet[1] = addr>>8; //leftmost 8 bits need to go in the first number, yes it is >> not <<
    packet[2] = addr&0xFF; //address, this is a reminder that we need to get the second half of the byte
    packet[3] = data[0]; //val we want to store
    packet[4] = data[1]; //val we want to store
    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, packet, 5); //data is a uint8_t array
    cs_deselect(PIN_CS_RAM);

}

void spi_ram_read(uint16_t addr, uint8_t *data, int len){
    uint8_t packet[5]; //outgoing data
    packet[0] = 0b00000011; //instruction, read
    packet[1] = addr>>8; //address we want to read from
    packet[2] = addr&0xFF; //address
    packet[3] = 0; 
    packet[4] = 0; 
    uint8_t dst[5]; //array of what we are reading, incoming data
    cs_select(PIN_CS_RAM);
    spi_write_read_blocking(SPI_PORT, packet, dst, 5); //where data is a uint8_t
    cs_deselect(PIN_CS_RAM);

    data[0] = dst[3]; //data came back, 1st byte we want 
    data[1] = dst[4]; //2nd byte we want

}