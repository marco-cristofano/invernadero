#ifndef _DTH22_H_
#define _DTH22_H_

#include "stdint.h"

//almacenamiento de las Humedad y Temperatura
volatile uint16_t HUMIDITY_RELATIVE;
volatile uint16_t TEMPERATURE;
volatile uint8_t FINISH; //si FINISH es igual a 1, finalizó el sensado 

void DHT22_handler(void);
void DTH22_sensing(void);
void DHT22_stop(void);


#endif

/*void DHT22_handler(void); Funcion que se encarga del tratamiento de la interrupcion del imput capture del TIMER0 para el DTH22. Se debe hacer su llamado desde la interrupcion. Para indicar la finalizacion de la interrpcion la varible GLOBAL FINISH se coloca en 1, a partir de ese instante en las variables globales TEMPERATURE y HUMIDITY_RELATIVE se encuentra almacenados los resultados. Si TEMPERATURE Y HUMIDITY_RELATIVE equivales a 65535 significa que el sendado fue erroneo(checksum failed)*/
/*void DTH22_sensing(void);  Funcion que envia el pulso de start hacia el DTH22 y luego configura todo lo necesario para el INPUTCAPTURE. Se debe llamar por cada ves que se requiera realziar el sensado*/
/*void DHT22_stop(void); Funcion que se llama en caso de que se desee deshabilitar completamente la captura de flancos del sistema*/

/*CONEXIONADO
EL voltage de alimentacion típico es de 5V(apto de 3.3V hasta 6V)
La comunicacion se realiza por medio de un solo puerto, PUERTO 66 o MDIO.
VER IMAGEN "conexionDHT221" y "conexionDHT22" para mas información
Se puede agregar un acapacitor de 100uF entre GND y VCC para obtener mayor estabilidad en el sensado
ACLARACION: en la imagen "conexionDHT222" el calble rojo es 5V, el negro es tierra y el blanco es DATOS

 */