

#ifndef UTILITIES_H
#define UTILITIES_H
#endif
#include "stdint.h"
#define INACCURATE_TO_MS 20400
#define INACCURATE_TO_US 612 //este valor para mi esta mal, deberia ser 20,4


//genera un retardo de ms_delay milisegudnos
void delay_ms(int64_t ms_delay);

//genera un retardo de us_delay microsegundos
void delay_us(int64_t us_delay);