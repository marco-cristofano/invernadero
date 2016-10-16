#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include "stdint.h"
void Keypad_scan(void); //Función que realiza un barrido del teclado con el objetivo de comprobar si se ha presionado alguna tecla
uint8_t Keypad_get_key(uint8_t *key);//Función que devuelve un 0 si ninguna tecla ha sido presionada, si devuelve un uno una tecla fue presionada y la misma se encuentra alamcenada en la variable pasada como parametro
#endif 

/*
MODO DE CONEXION DEL KEYPAD(4X4) EN EDU-CIAA NXP

FILAS=SALIDAS
FILA1= GPIO0
FILA2= GPIO1
FILA3= GPIO2
FILA4= GPIO3

COLUMNAS=ENTRADAS
COLUMNA1=GPIO4
COLUMNA2=GPIO5
COLUMNA3=GPIO6
COLUMNA4=GPIO7

MODO DE USO DEL KEYPAD(4X4) EN EDU-CIAA NXP

while( 1 ) {
    delay_ms(250); SE DEBE REALIZAR EL SCANER CON UNA FRECUENCIA DE 4 VECES POR SEGUNDO
    Keypad_scan();
    UINT8_T aux=Keypad_get_key(&tecla); ADQUIERO LA TECLA SI HA SIDO PRESIONADA, EN TECLA SE ALAMCENO LA TECLA SIEMPRE Y CUANDO AUX ES 1, CASO CONTRARIO AUX ES 0 SIGNIFICA QUE NO SE HA PRESIONADO NINGUNA TECLA
       
   }

   */