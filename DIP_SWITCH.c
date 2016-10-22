#include "DIP_SWITCH.h"
#include "chip.h"
#include "cmsis.h"
#include "utilities.h"


#define MAX_SWITCHS 10//Cantida de dispositivos maxima que permite el DIP_SWITCH
#define MAX_COLUMNS 5 //Cantidad de columnas de la matriz(SCU_PORT, SCU_PIN_, GPIO_PORT_ GPIO_PIN y FUNCTION)   
#define SCU_PORT    0 //Posicion(columna) de SCU_PORT en la matriz 
#define SCU_PIN     1 //Posicion(columna) de SCU_PIN en la matriz 
#define GPIO_PORT   2 //Posicion(columna) de GPIO_PORT en la matriz 
#define GPIO_PIN    3 //Posicion(columna) de GPIO_PIN en la matriz 
#define FUNC        4 //Posicion(columna) de FUNCTION en la matriz 



static volatile uint8_t CONECTED_DEVICES[MAX_SWITCHS];

//matriz que almacena el SCU_PORT, SCU_PIN_, GPIO_PORT_ GPIO_PIN y FUNCTION de cada pin utlizado para el DIP_SWTICH
static uint8_t switch_data[MAX_SWITCHS][MAX_COLUMNS]={ {6,1,3,0,0}, {6,4,3,3,0}, {6,5,3,4,0}, {6,7,5,15,4}, 
                        {6,8,5,16,4}, {6,9,3,5,0}, {6,10,3,6,0}, {6,11,3,7,0}, 
                        {6,12,2,8,0}, {4,0,2,0,0} }; 

void DIPSWITCH_init(void){
   uint8_t i;
  /* Config Dip Switch Pins */
   for(i=0; i<MAX_SWITCHS;i++){
      if(switch_data[i][FUNC]==0){
         Chip_SCU_PinMux(switch_data[i][SCU_PORT], switch_data[i][SCU_PIN], MD_PUP, FUNC0); 
         Chip_GPIO_SetDir(LPC_GPIO_PORT, switch_data[i][GPIO_PORT], (1<<switch_data[i][GPIO_PIN]), INPUT);
      }
      else{
         Chip_SCU_PinMux(switch_data[i][SCU_PORT], switch_data[i][SCU_PIN], MD_PUP, FUNC4);
         Chip_GPIO_SetDir(LPC_GPIO_PORT, switch_data[i][GPIO_PORT], (1<<switch_data[i][GPIO_PIN]), INPUT); 
      }
   }

}
void DIPSWITCH_sensing(void){
int8_t i; 
for(i=0; i<MAX_SWITCHS;i++){
    if(Chip_GPIO_GetPinState(LPC_GPIO_PORT, switch_data[i][GPIO_PORT], switch_data[i][GPIO_PIN])==LOW)//si esta coenctado
      CONECTED_DEVICES[i]=ON;
    else
      CONECTED_DEVICES[i]=ON;
  }
}

uint8_t DIPSWITCH_askDevice(uint8_t indice){
    return CONECTED_DEVICES[indice];

}