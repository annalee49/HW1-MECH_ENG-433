#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hx711_0.h"

#define clock_time_us 50


int main()
{
    stdio_init_all();

    init_hx711();
    sleep_ms(2000);


    int i = 0;
    uint64_t last_t = 0;

    while (true) {
        //char m[100];
        int v[1000];
        int num = 0;
        //int raw[1000];
        //float filt[1000];
        uint64_t t[1000];
        scanf("%d", &num);
        int filt_v[1000];

        int avg = hx711_read_raw();
        for (i=0; i<num; i++){
            int val = hx711_read_raw();
            avg = val * 0.1 + avg * 0.9;
            v[i] = val;
            filt_v[i] = avg;
            t[i] = to_ms_since_boot(get_absolute_time());
        }
        for (i=0; i<num; i++){
            printf("%d %llu %d %d\n", i, t[i], v[i], filt_v[i]);
        }
    }
}
