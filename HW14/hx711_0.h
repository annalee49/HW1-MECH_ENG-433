#ifndef HX711_0
#define HX711_0

#define PIN_DOUT 15
#define PIN_PD_SCK 14

#include <stdio.h>
#include "pico/stdlib.h"

void init_hx711();
int hx711_read_raw(); 

#endif