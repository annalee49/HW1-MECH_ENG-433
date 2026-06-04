#include "hx711_0.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>

#define clock_time_us 50
#define PIN_PD_SCK 14
#define PIN_DOUT 15

void init_hx711(){
    gpio_init(PIN_DOUT);
    gpio_set_dir(PIN_DOUT, GPIO_IN);
    gpio_pull_up(PIN_DOUT);

    gpio_init(PIN_PD_SCK);
    gpio_set_dir(PIN_PD_SCK, GPIO_OUT);
    gpio_put(PIN_PD_SCK,0);
}

int hx711_read_raw(void){
    //wait for data ready (DOUT is low)
    // while(gpio_get(PIN_DOUT)){
    //     tight_loop_contents();
    // }

    int timeout = 1000000;
    while (gpio_get(PIN_DOUT)) {
        tight_loop_contents();
    }
    
    if (timeout <= 0) {
        printf("HX711 timeout\r\n");
        return 0;
    }

    unsigned int raw = 0;
    for (int i = 0; i<24; i++){
        gpio_put(PIN_PD_SCK, 1);
        sleep_us(clock_time_us); //short settle
        raw = (raw << 1) | (gpio_get(PIN_DOUT) ? 1 : 0);
        gpio_put(PIN_PD_SCK,0);
        sleep_us(clock_time_us);
    }

    //25th pulse to set gain = 128 for next reading
    gpio_put(PIN_PD_SCK, 1);
    sleep_us(clock_time_us);
    gpio_put(PIN_PD_SCK, 0);

    //sign extend 24 bit two's complement to 32 bit signed int
    if (raw & 0x800000) {
        raw |= 0xFF000000;
    }
    return (int)raw;

}

void hx711_debug_bits(void) {
    // wait until ready (optional but helpful)
    int timeout = 1000000;
    while (gpio_get(PIN_DOUT) && timeout--) {
        sleep_us(10);
    }
    if (timeout <= 0) {
        printf("timeout\n");
        return;
    }
    printf("BITS: ");

    for (int i = 0; i < 24; i++) {
        gpio_put(PIN_PD_SCK, 1);
        sleep_us(100);

        int bit = gpio_get(PIN_DOUT);
        printf("%d", bit);

        gpio_put(PIN_PD_SCK, 0);
        sleep_us(100);
    }

    printf("\n");

    // extra pulse (required by HX711 protocol)
    gpio_put(PIN_PD_SCK, 1);
    sleep_us(100);
    gpio_put(PIN_PD_SCK, 0);

    sleep_ms(200);
}