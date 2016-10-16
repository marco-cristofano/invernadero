#include "Keypad.h"
#include "chip.h"
#include "cmsis.h"

#define OUTPUT           1
#define INPUT            0
#define HIGH             1
#define LOW              0   

//definiciones para el teclado
//salidas=filas
//fila1
#define SCU_PORT_F1OUT    6
#define SCU_PIN__F1OUT    1
#define GPIO_PORT_F1OUT   3
#define GPIO_PIN_F1OUT    0 
//fila2
#define SCU_PORT_F2OUT    6
#define SCU_PIN__F2OUT    4
#define GPIO_PORT_F2OUT   3
#define GPIO_PIN_F2OUT    3 
//fila3
#define SCU_PORT_F3OUT    6
#define SCU_PIN__F3OUT    5
#define GPIO_PORT_F3OUT   3
#define GPIO_PIN_F3OUT    4 
//fila4
#define SCU_PORT_F4OUT    6
#define SCU_PIN__F4OUT    7
#define GPIO_PORT_F4OUT   5
#define GPIO_PIN_F4OUT    15 

//entradas=columnas
//columna1
#define SCU_PORT_C1IN     6
#define SCU_PIN__C1IN     8
#define GPIO_PORT_C1IN    5
#define GPIO_PIN_C1IN     16 
//columna2
#define SCU_PORT_C2IN     6
#define SCU_PIN__C2IN     9
#define GPIO_PORT_C2IN    3
#define GPIO_PIN_C2IN     5 
//columna3
#define SCU_PORT_C3IN     6
#define SCU_PIN__C3IN     10
#define GPIO_PORT_C3IN    3
#define GPIO_PIN_C3IN     6
//columna4
#define SCU_PORT_C4IN     6
#define SCU_PIN__C4IN     11
#define GPIO_PORT_C4IN    3
#define GPIO_PIN_C4IN     7 

static uint8_t KeyChanged;
static uint8_t valid_key;

void Keypad_init(void)
{
//INICIALIZACION DE LOS PINES PARA SALIDA
//FILA1
Chip_SCU_PinMux ( SCU_PORT_F1OUT , SCU_PIN__F1OUT , SCU_MODE_INACT, FUNC0);
Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_F1OUT  , GPIO_PIN_F1OUT, OUTPUT);
//FILA2
Chip_SCU_PinMux ( SCU_PORT_F2OUT, SCU_PIN__F2OUT, SCU_MODE_INACT, FUNC0);
Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_F2OUT, GPIO_PIN_F2OUT, OUTPUT);
//FILA3
Chip_SCU_PinMux ( SCU_PORT_F3OUT, SCU_PIN__F3OUT, SCU_MODE_INACT, FUNC0);
Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_F3OUT, GPIO_PIN_F3OUT, OUTPUT);
//FILA4
Chip_SCU_PinMux ( SCU_PORT_F4OUT, SCU_PIN__F4OUT, SCU_MODE_INACT, FUNC4);
Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_F4OUT, GPIO_PIN_F4OUT, OUTPUT);

//INICIALIZACION DE LOS PINES PARA ENTRADA
//COLUMNA1
Chip_SCU_PinMux ( SCU_PORT_C1IN , SCU_PIN__C1IN , MD_PUP|MD_EZI , FUNC4);
Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_C1IN  , GPIO_PIN_C1IN, INPUT);
//COLUMNA2
Chip_SCU_PinMux ( SCU_PORT_C2IN , SCU_PIN__C2IN , MD_PUP|MD_EZI , FUNC0);
Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_C2IN  , GPIO_PIN_C2IN, INPUT);
//COLUMNA3
Chip_SCU_PinMux ( SCU_PORT_C3IN , SCU_PIN__C3IN , MD_PUP|MD_EZI , FUNC0);
Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_C3IN  , GPIO_PIN_C3IN, INPUT);
//COLUMNA4
Chip_SCU_PinMux ( SCU_PORT_C4IN, SCU_PIN__C4IN , MD_PUP|MD_EZI , FUNC0);
Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, GPIO_PORT_C4IN  , GPIO_PIN_C4IN, INPUT);

}


static uint8_t scan_keypad1(uint8_t *key){
  unsigned char i;
  bool C1,C2,C3,C4;
  for(i=0; i<4; i++){  //Coloca p/c iteración un GPIO en cero y el resto en 1
    if(i==0){
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F1OUT, GPIO_PIN_F1OUT, LOW);
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F2OUT, GPIO_PIN_F2OUT, HIGH);
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F3OUT, GPIO_PIN_F3OUT, HIGH);
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F4OUT, GPIO_PIN_F4OUT, HIGH);
    }
    else if(i==1){
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F1OUT, GPIO_PIN_F1OUT, HIGH);
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F2OUT, GPIO_PIN_F2OUT, LOW);
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F3OUT, GPIO_PIN_F3OUT, HIGH);
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F4OUT, GPIO_PIN_F4OUT, HIGH);
    }
    else if(i==2){
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F1OUT, GPIO_PIN_F1OUT, HIGH);
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F2OUT, GPIO_PIN_F2OUT, HIGH);
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F3OUT, GPIO_PIN_F3OUT, LOW);
     Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F4OUT, GPIO_PIN_F4OUT, HIGH);
    }
    else{ 
    Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F1OUT, GPIO_PIN_F1OUT, HIGH);
    Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F2OUT, GPIO_PIN_F2OUT, HIGH);
    Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F3OUT, GPIO_PIN_F3OUT, HIGH);
    Chip_GPIO_SetPinState( LPC_GPIO_PORT, GPIO_PORT_F4OUT, GPIO_PIN_F4OUT, LOW);
   }

   C1=Chip_GPIO_ReadPortBit  ( LPC_GPIO_PORT,GPIO_PORT_C1IN,GPIO_PIN_C1IN);
   C2=Chip_GPIO_ReadPortBit  ( LPC_GPIO_PORT,GPIO_PORT_C2IN,GPIO_PIN_C2IN);
   C3=Chip_GPIO_ReadPortBit  ( LPC_GPIO_PORT,GPIO_PORT_C3IN,GPIO_PIN_C3IN);
   C4=Chip_GPIO_ReadPortBit  ( LPC_GPIO_PORT,GPIO_PORT_C4IN,GPIO_PIN_C4IN);


  if( C1==LOW || C2==LOW || C3==LOW || C4==LOW  ){ //si apreto una tecla
      //identifico la tecla apretada
     if(i==0){ //Si la tecla apretada pertenece a la fila 1
        if(C1 == LOW)
          *key='1';
        else if(C2==LOW)
          *key='2';
         else if(C3==LOW)
          *key='3';
        else if(C4==LOW)
          *key='A'; 
        return 1;
      }
      else if(i==1){ //Si la tecla apretada pertenece a la fila 2
        if(C1==LOW)
          *key='4';
        else if(C2==LOW)
          *key='5';
        else if(C3==LOW)
          *key='6';
        else if(C4==LOW)
          *key='B';
        return 1;
      }
      else if(i==2){ //Si la tecla apretada pertenece a la fila 3
        if(C1==LOW)
          *key='7';
        else if(C2==LOW)
          *key='8';
        else if(C3==LOW)
          *key='9';
        else if(C4==LOW)
          *key='C'; 
        return 1;
      }
      else if(i==3){ //Si la tecla apretada pertenece a la fila 4
        if(C1==LOW)
          *key='*'; //14=*
        else if(C2==LOW)
          *key='0';
        else if(C3==LOW)
          *key='#';
        else if(C4==LOW)
          *key='D';
        return 1;
      }
    }
  }
  return 0;
  
}

void Keypad_scan(void){
  
  static uint8_t current_state=0, last_state=0, key, free_key=0;
  uint8_t aux_key;
  
  current_state=scan_keypad1(&aux_key);
  
  if(current_state==0 && last_state==1) //Se determina si se solto la tecla
    free_key=1; //si soltó la tecla
  else
    free_key=0; // sigue apretahdo la tecla
  
  last_state=current_state;

  if(free_key==1){  //Si se soltó se coloca la tecla como válida
    KeyChanged=1;//flag qeu avisa que cambio la tecla
    valid_key=key; //coloca en el buffer la tecla que corresponde
  }
  key=aux_key;//se almacena el valor nuevo
}

uint8_t Keypad_get_key(uint8_t *key){
  
  if(!KeyChanged)
    return 0;
  else{
    KeyChanged=0;
    *key=valid_key;
    return 1;
  } //se devuelve la tecla si no fue tomada antes
//evita que se tome una tecla 2 veces
}
