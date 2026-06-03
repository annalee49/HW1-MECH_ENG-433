#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include <stdlib.h>

#define clock_time_us 50

#define PIN_PD_SCK 19
#define PIN_DOUT 20


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
    while (gpio_get(PIN_DOUT) && timeout--) {
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

int main()
{
    stdio_init_all();

    init_hx711();
    sleep_ms(2000);
    printf("HX711 ready\r\n");

    int i = 0;
    uint64_t last_t = 0;
    
    while (true) {
        char m[100];
        int v[1000];
        //int num = 0;
        int num;
        uint64_t t[1000];
        //scanf("%d", &num);

        char buf[32];
        fgets(buf, sizeof(buf), stdin);
        num = atoi(buf);

        float avg = 828000.0f;
        for (i=0; i<num; i++){
            int val = hx711_read_raw();
            avg = val * 0.1f + avg * 0.9f;
            v[i] = avg;
            t[i] = to_ms_since_boot(get_absolute_time());
        }
        for (i=0; i<num; i++){
            printf("%d,%d,%llu\n", v[i], v[i], t[i]);
        }
    }
}
