#ifndef _DIP_SWITCH_H_
#define _DIP_SWITCH_H_

#include "stdint.h"

//almacenamiento del estado de los dispositivos conectados

void DIPSWITCH_init(void);
void DIPSWITCH_sensing(void);
uint8_t DIPSWITCH_askDevice(uint8_t indice);

#endif

/*void DIPSWITCH_init(void); Funcion que realiza la configuracion de los pines destinados a la conexion con el DIPSWITCH*/
/*void DIPSWITCH_sensing(void); Funcion que actualiza la cantidad de dispositivos conectados*/
/*uint8_t DIPSWITCH_askDevice(uint8_t indice); Funcion que retorna con ON si el dispositivo de la ubicacion indice se encuantra actualmente conectado*/