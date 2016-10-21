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
#define TIME_0_MIN          4488  //22Us 
#define TIME_0_MAX          6528  //32Us
#define TIME_1_MIN          13872 //68Us
#define TIME_1_MAX          15300 //75Us

//varibles para el funcionamiento logico
volatile static volatile uint8_t  checksum;;
volatile static uint32_t subtract=0;
volatile static uint8_t location=0;
volatile static uint32_t last_edge;
volatile static uint32_t current_edge;
 

/* Para llamar a esta funcion debemos tener el siguiete codigo, recordar modificar el vector de interrupciones
__attribute__ ((section(".after_vectors")))
void TIMER0_IRQHandler(void){ 
    save_bit();
}
*/

void DHT22_handler(void){
  //---Variables-------------------------------------------
  uint8_t sum;
  FINISH=0;
  //-------------------------------------------------------
   current_edge=Chip_TIMER_ReadCapture(LPC_TIMER, CAP_NUMB);//recojo el valor de la captura
   if(current_edge > last_edge)//si nose desborda el contador
        subtract=current_edge - last_edge;
   else//si se desborda el contador
        subtract=(MAXVALUE - last_edge + current_edge);
   last_edge=current_edge;//se actualiza el EDGE anterior
   
   if (subtract <= TIME_0_MAX && subtract >= TIME_0_MIN){ //si entra es por que es un 0
      //Guardo un cero donde corresponda
        if (location >= 0 && location <= 15)//el bit pertenece a la HUMEDAD RELATIVA
          HUMIDITY_RELATIVE=(HUMIDITY_RELATIVE<<1);
         else if (location>=16 && location<=31) //el bit pertenece a la TEMPERATURA
           TEMPERATURE=(TEMPERATURE<<1);
         else //el bit pertenece al Checksum
           checksum=(checksum<<1); 
        location++;    
    }
       else if (subtract <= TIME_1_MAX && subtract >= TIME_1_MIN){ //si entra es por que es un 1
        //Guardo un 1 donde corresonda
         if (location >= 0 && location <= 15)///el bit pertenece a la HUMEDAD RELATIVA
           HUMIDITY_RELATIVE=(HUMIDITY_RELATIVE<<1) | 1;
         else if (location>=16 && location<=31)//el bit pertenece a la TEMPERATURA
           TEMPERATURE=(TEMPERATURE<<1) | 1;
         else //el bit pertence al Checksum
           checksum=(checksum<<1) | 1;   
          location++;   
    }
  if (location==40){//se terminó el frame que envia el DHT22
    sum=(HUMIDITY_RELATIVE>>8)+HUMIDITY_RELATIVE+(TEMPERATURE>>8)+TEMPERATURE; //suma necesaria para calcular el checksum
    if(checksum != sum){
      HUMIDITY_RELATIVE=65535;     
      TEMPERATURE=65535;
    }
    FINISH=1;//indicacion de finalizacion del sensado
    location=0;
    last_edge=0;
    Chip_TIMER_Disable(LPC_TIMER0);  
  }
 Chip_TIMER_ClearCapture(LPC_TIMER, CAP_NUMB);//limpia la captura ya procesada

}
void DTH22_sensing(void){
//Envio del pulso de start----------------------------------------------- 
Chip_SCU_PinMux ( IC_SCU_PORT, IC_SCU_PIN, MD_EZI | MD_PUP, FUNC0);//seleccion del puerto 66 como GPIO
Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_OUT, GPIO_PIN_OUT, OUTPUT);//seleccion del puerto 66(cpio0[12] como salida)
Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_OUT, GPIO_PIN_OUT, LOW);//pulso en bajo
delay_ms(17);//REVISAR VALOr EN EL MANUAL(ES CONFUSO)
Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_OUT, GPIO_PIN_OUT, HIGH);//pulso en alto(innecesario teniendo en cuenta la presencia del PULL-UP)
//Fin del Envio del pulso inicial----------------------------------------
Chip_Clock_Enable(CLK_MX_TIMER0);//habilita el clock para el TIMER0
Chip_SCU_PinMux ( IC_SCU_PORT, IC_SCU_PIN,MD_EZI | MD_PUP, FUNC4);//Pin 66 en modo captura
Chip_TIMER_Init(LPC_TIMER);//inicializa el timer      
LPC_GIMA->CAP0_IN[TIMER_NUMBER][CAP_NUMB] = ((1<<4));//
Chip_RGU_TriggerReset(RGU_TIMER_RST);//llama al sistema de reset(RGU) para que resetee el timer0
while (Chip_RGU_InReset(RGU_TIMER_RST)); //esta funcion devuelve true si todavia se está reseteando, por lo tanto sale del while cuando se termino el reset
Chip_TIMER_Reset(LPC_TIMER); //resetea el timer desde el propio timer
Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER, TIMER_CAPSRC_RISING_PCLK, CAP_NUMB); 
Chip_TIMER_PrescaleSet(LPC_TIMER, TIMER0_PRESCALER);//setea el prescalador
Chip_TIMER_ClearCapture(LPC_TIMER, CAP_NUMB);//elimina captura antigua
Chip_TIMER_CaptureRisingEdgeEnable(LPC_TIMER, CAP_NUMB);//activa la captura de flancos ascendentes
Chip_TIMER_CaptureFallingEdgeEnable (LPC_TIMER, CAP_NUMB );//Activo la captura de flanco de bajada
Chip_TIMER_CaptureEnableInt(LPC_TIMER, CAP_NUMB);//cada vez que el CAP0 sea cargado el timer lanzara una interrpcion
NVIC_ClearPendingIRQ(LPC_TIMER_IRQ);//limpia las interrupciones del timer que pudieron haber quedado por un previo uso(supongo que no hace falta)
NVIC_EnableIRQ(LPC_TIMER_IRQ);//activa las interrupciones del timer
Chip_TIMER_Enable(LPC_TIMER);//activa el timer
}

void DHT22_stop(void){ 
  Chip_Clock_Disable(CLK_MX_TIMER0);
  NVIC_DisableIRQ(LPC_TIMER_IRQ);
}