#include "DTH22.h"
#include "chip.h"
#include "cmsis.h"
#include "utilities.h"

//definiciones para el puerto del timer
#define IC_SCU_PORT         1
#define IC_SCU_PIN          17
#define GPIO_PORT_OUT       0   
#define GPIO_PIN_OUT        12

//definiciones para el timer
#define TIMER_NUMBER        0
#define CAP_NUMB            3
#define TIMER0_PRESCALER    0
#define LPC_TIMER            LPC_TIMER0
#define RGU_TIMER_RST        RGU_TIMER0_RST 
#define LPC_TIMER_IRQ        TIMER0_IRQn

//definir con cuando este el clock
#define MAXVALUE            4294967296 
#define TIME_0_MIN          3060  //15Us 
#define TIME_0_MAX          4140  //35Us
#define TIME_1_MIN          12240 //60Us
#define TIME_1_MAX          15096 //74Us


 uint32_t subtract=0;
 volatile  uint8_t location=0;
 volatile  uint8_t ubication=0; //borrar
 volatile static uint32_t last_edge;
 volatile static uint32_t current_edge;

 //volatile uint8_t information[5]
 volatile uint8_t HR_integer_byte;
 volatile uint8_t HR_decimal_byte; 
 volatile uint8_t T_integer_byte;
 volatile uint8_t T_decimal_byte;
 volatile uint8_t checksum;


/* Para llamar a esta funcion debemos tener el siguiete codigo, recordar modificar el vector de interrupciones
__attribute__ ((section(".after_vectors")))
void TIMER0_IRQHandler(void){ 
    save_bit();
    Chip_TIMER_ClearCapture(LPC_TIMER, CAP_NUMB);
}
*/

void DHT22_stop(void){
	Chip_Clock_Disable(CLK_MX_TIMER0);
    NVIC_DisableIRQ(LPC_TIMER_IRQ);
}


void DHT22_handler(void){
	current_edge=Chip_TIMER_ReadCapture(LPC_TIMER, CAP_NUMB);
    if(current_edge > last_edge)//si nose desborda el contador
        subtract=current_edge - last_edge;
    else//si se desborda el contador
        subtract=(MAXVALUE - last_edge + current_edge);
    last_edge=current_edge;
    
    ubication++; //BORRAR

    if (subtract <= TIME_0_MAX && subtract >= TIME_0_MIN){ //si entra es por que es un 0
      //Guardo un cero donde corresponda
         if (location >= 0 && location <= 7)//el bit pertenece a la parte entera de HR
           HR_integer_byte=(HR_integer_byte<<1);
         else if (location>=8 && location<=15) //El bit pertenece a la parte decimal de la HR
           HR_decimal_byte=(HR_decimal_byte<<1);
         else if ((location >= 16) && (location <= 23))//el bit pertenece a la parte entera de T
           T_integer_byte=(T_integer_byte<<1);
         else if (location>=24 && location <=31) //el bit pertenece a la parte decimal de T
           T_decimal_byte=(T_decimal_byte<<1);
         else //el bit pertenece al Checksum
           checksum=(checksum<<1); 
        location++;    
    }
       else if (subtract <= TIME_1_MAX && subtract >= TIME_1_MIN){ //si entra es por que es un 1
         //Guardo un 1 donde corresonda
         if (location >= 0 && location <= 7)//el bit pertence a la parte entera de HR
           HR_integer_byte=(HR_integer_byte<<1) | 1;
         else if (location>=8 && location<=15)//El bit pertenece a la parte decimal de la HR
           HR_decimal_byte=(HR_decimal_byte<<1) | 1;
         else if (location >= 16 && location <= 23)//el bit pertence a la parte entera de TEMP
           T_integer_byte=(T_integer_byte<<1) | 1;
         else if (location>=24 && location <=31) //el bit pertence a la parte decimal de TEMP
           T_decimal_byte=(T_decimal_byte<<1) | 1;
         else //el bit pertence al Checksum
           checksum=(checksum<<1) | 1;   
          location++;   
    }
  if (location==40){//se terminó el frame que envia el DHT22
    location=0;
    last_edge=0;
    Chip_TIMER_Disable(LPC_TIMER0);  
  }

}
void DTH22_init(void){
	Chip_Clock_Enable(CLK_MX_TIMER0);
    Chip_SCU_PinMux ( IC_SCU_PORT, IC_SCU_PIN,MD_EZI | MD_PUP, FUNC4);
    /* Timer0 init */
    Chip_TIMER_Init(LPC_TIMER);//inicializa el timer      
    /* Timer 0 CAP0 EDGE enabled and SELECT as 0x2 */
    LPC_GIMA->CAP0_IN[TIMER_NUMBER][CAP_NUMB] = ((1<<4));
    /* Reset timer0 */
    Chip_RGU_TriggerReset(RGU_TIMER_RST);//llama al sistema de reset(RGU) para que resetee el timer0
    
    while (Chip_RGU_InReset(RGU_TIMER_RST)); //esta funcion devuelve true si todavia se está reseteando, por lo tanto sale del while cuando se termino el reset

    Chip_TIMER_Reset(LPC_TIMER); //resetea el timer desde el propio timer
    
    Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER, TIMER_CAPSRC_RISING_PCLK, CAP_NUMB); 
    /* Select prescaler value */
    Chip_TIMER_PrescaleSet(LPC_TIMER, TIMER0_PRESCALER);//setea el prescalador
    /* Clear capture value on T0.CAP0 */
    Chip_TIMER_ClearCapture(LPC_TIMER, CAP_NUMB);//elimina captura antigua
    /* Enable rising trigger on T0.CAP0 */
    Chip_TIMER_CaptureRisingEdgeEnable(LPC_TIMER, CAP_NUMB);//activa la captura de flancos ascendentes
    Chip_TIMER_CaptureFallingEdgeEnable (LPC_TIMER, CAP_NUMB );//Activo la captura de flanco de bajada
    Chip_TIMER_CaptureEnableInt(LPC_TIMER, CAP_NUMB);//cada vez que el CAP0 sea cargado el timer lanzara una interrpcion
    /* Enable timer interrupt */
    NVIC_ClearPendingIRQ(LPC_TIMER_IRQ);//limpia las interrupciones del timer que pudieron haber quedado por un previo uso(supongo que no hace falta)
    NVIC_EnableIRQ(LPC_TIMER_IRQ);//activa las interrupciones del timer
    /* Enable timer */
    Chip_TIMER_Enable(LPC_TIMER);

}
void DTH22_startPulse(void){

	Chip_SCU_PinMux ( IC_SCU_PORT, IC_SCU_PIN, MD_EZI, FUNC0);
	Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_OUT, GPIO_PIN_OUT, OUTPUT);
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_OUT, GPIO_PIN_OUT, LOW);
	delay_ms(18);//REVISAR VALRO EN EL MANUAL(ES CONFUSO)
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_OUT, GPIO_PIN_OUT, HIGH);


}