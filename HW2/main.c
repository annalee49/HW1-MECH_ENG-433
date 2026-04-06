#include <stdio.h>                    // standard C library
#include "pico/stdlib.h"              // core Pico SDK
#include "pico/platform.h"            // required by stdlib
#include "hardware/gpio.h"            // for gpio_set_function
#include "hardware/pwm.h"             // PWM control
#include "hardware/adc.h"             // ADC input
#include "pico/cyw43_arch.h"          // only if you use Wi-Fi (2W only)

#define PWMPIN 16

bool timer_interrupt_function(__unused struct repeating_timer *t) {
    // read the adc
    uint16_t result1 = adc_read();
    // print the voltage
    printf("%f\r\n",(float)result1/4095*3.3);
    return true;
}

int main()
{
    stdio_init_all();

    // turn on the adc first
    adc_init();
    adc_gpio_init(26); // pin GP26 is pin ADC0
    adc_select_input(0); // sample from ADC0

    // turn on a timer interrupt
    struct repeating_timer timer;
    // -100 means call the function every 100ms
    add_repeating_timer_ms(-100, timer_interrupt_function, NULL, &timer);

    // turn on the pwm
    gpio_set_function(PWMPIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWMPIN);
    float div = 10;
    pwm_set_clkdiv(slice_num, div);
    uint16_t wrap = 1500;
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    pwm_set_gpio_level(PWMPIN, 1500/2);

    while (true) {
        tight_loop_contents(); // do nothing here, the interrupt does the work
    }
}