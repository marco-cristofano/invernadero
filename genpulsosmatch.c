/*==================[inclusions]=============================================*/
#include "os.h"               /* <= operating system header */
#include "ciaak.h"            /* <= ciaa kernel header */
#include "ciaaPOSIX_stdio.h"  /* <= device handler header */
#include "ciaaPOSIX_string.h" /* <= string header */
#include "chip.h"
#include "stdint.h"
#include "genpulsosmatch.h" 
#include "utilities.h"
/*==================[macros and definitions]=================================*/


//definiciones para el puerto de salida
#define SCU_PORT_PINOUT    6
#define SCU_PIN__PINOUT    4
#define GPIO_PORT_PINOUT   3
#define GPIO_PIN_PINOUT    3

#define SCU_PORT_MATCH    1
#define SCU_PIN__MATCH    16
#define GPIO_PORT_MATCH   3
#define GPIO_PIN_MATCH    3

#define RGU_TIMER_RST       RGU_TIMER0_RST 
#define MATCHNUMBER       0
#define timerNumber       0
#define LPC_TIMER            LPC_TIMER0
#define TIMERCOMPAREMATCH0 0|
#define LPC_TIMER_IRQ       TIMER0_IRQn

#define LPC4337_MAX_FREC 204000000 /* Microcontroller frequency */
#define MAX_SYSCALL_INTERRUPT_PRIORITY 5

/*==================[internal data declaration]==============================*/
uint8_t salida = 0; 

int main(void)
{
   /* Starts the operating system in the Application Mode 1 */
   /* This example has only one Application Mode */
   StartOS(AppMode1);

   /* StartOs shall never returns, but to avoid compiler warnings or errors
    * 0 is returned */
   return 0;
}

void ErrorHook(void)
{
   while(1);
}

uint32_t Timer_microsecondsToTicks(uint32_t uS){
   return (uS*(LPC4337_MAX_FREC/1000000));
}

void pulso(void){
 if (salida == 0)
  Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_PINOUT, GPIO_PIN_PINOUT, HIGH);
 else
  Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_PINOUT, GPIO_PIN_PINOUT, LOW);
}




void Timer_Init(uint32_t ticks)
{
   
    Chip_Clock_Enable(CLK_MX_TIMER0);
    Chip_SCU_PinMux ( SCU_PORT_MATCH, SCU_PIN__MATCH,MD_EZI, FUNC4);

  
   Chip_TIMER_Init(LPC_TIMER);
   Chip_RGU_TriggerReset(RGU_TIMER_RST);
   while (Chip_RGU_InReset(RGU_TIMER_RST));
   Chip_TIMER_Reset(LPC_TIMER); //resetea el timer desde el propio timer
   Chip_TIMER_ExtMatchControlSet(LPC_TIMER, 0,TIMER_EXTMATCH_TOGGLE,MATCHNUMBER);
   
   Chip_TIMER_PrescaleSet(LPC_TIMER, 0);
   
   Chip_TIMER_SetMatch(LPC_TIMER, 0,ticks);
   Chip_TIMER_ClearMatch(LPC_TIMER,MATCHNUMBER);
   
   Chip_TIMER_ResetOnMatchEnable(LPC_TIMER,0);
   
   
   //NVIC_ClearPendingIRQ(LPC_TIMER_IRQ);//limpia las interrupciones del timer que pudieron haber quedado por un previo uso(supongo que no hace falta)
   //NVIC_EnableIRQ(LPC_TIMER_IRQ);
   Chip_TIMER_Enable(LPC_TIMER); 
}

/**
 * @brief    main routine for blinky example
 * @return    Function should not exit.
 */
TASK(InitTask){  
   uint32_t us;
   ciaak_start();
   us=Timer_microsecondsToTicks(72);
   Timer_Init(us);
   while(1);

   TerminateTask();
}
