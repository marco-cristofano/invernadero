#include "utilities.h"
#include "stdint.h"



//genera un retardo de ms_delay milisegudnos
void delay_ms(int64_t ms_delay) {
   volatile int64_t i;
   volatile int64_t delay;
   delay = INACCURATE_TO_MS * ms_delay;
   for( i=delay; i>0; i-- );
}

//genera un retardo de us_delay microsegundos
void delay_us(int64_t us_delay) {
   volatile int64_t i;
   volatile int64_t delay;
   delay = INACCURATE_TO_US * us_delay;
   for( i=delay; i>0; i-- );
}