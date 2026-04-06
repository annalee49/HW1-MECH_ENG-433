#include <stdio.h> // set pico_enable_stdio_usb to 1 in CMakeLists.txt 
#include "pico/stdlib.h" // CMakeLists.txt must have pico_stdlib in target_link_libraries
#include "hardware/pwm.h" // CMakeLists.txt must have hardware_pwm in target_link_libraries
#include "hardware/adc.h" // CMakeLists.txt must have hardware_adc in target_link_libraries

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

    // turn on a timer interrupt
    struct repeating_timer timer;
    // -100 means call the function every 100ms
    // +100 would mean call the function 100ms after the function has ended
    add_repeating_timer_ms(-100, timer_interrupt_function, NULL, &timer);

    // turn on the pwm, in this example to 10kHz with a resolution of 1500
    gpio_set_function(PWMPIN, GPIO_FUNC_PWM); // Set the Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(PWMPIN); // Get PWM slice number
    // the clock frequency is 150MHz divided by a float from 1 to 255
    float div = 150.0f; // must be between 1-255, this is a chosen value
    uint16_t wrap = 19999; // when to rollover, must be less than 65535, set to this because of the 20 ms period
    
    pwm_set_clkdiv(slice_num, div); // sets the clock speed
    pwm_set_wrap(slice_num, wrap); 
    pwm_set_enabled(slice_num, true); // turn on the PWM

    uint16_t min_level = 1000; //5% duty cycle, 1 ms
    uint16_t max_level = 8000; //30% duty cycle, 6 ms

    pwm_set_gpio_level(PWMPIN, 1500/2); // set the duty cycle to 50% (old)

    // turn on the adc
    adc_init();
    adc_gpio_init(26); // pin GP26 is pin ADC0
    adc_select_input(0); // sample from ADC0

    uint16_t level = min_level; //start at min
    int8_t step = 10; //trying this granularity for sweep
    
    while (true) {
        tight_loop_contents(); // do nothing here, the interrupt does the work
        pwm_set_gpio_level(PWMPIN, level);
        level += step;

        if (level >= max_level || level <= min_level) {
            step = -step;
        }

        sleep_ms(20); //20 ms between each step
    }
}