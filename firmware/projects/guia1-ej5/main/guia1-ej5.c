/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * Escribir una función que reciba como parámetro un dígito BCD y un vector de estructuras 
 * del tipo gpioConf_t. Incluya el archivo de cabecera gpio_mcu.h
 * 
 * Defina un vector que mapee los bits de la siguiente manera:
 * b0 -> GPIO_20 
 * b1 -> GPIO_21 
 * b2 -> GPIO_22 
 * b3 -> GPIO_23 
 * La función deberá cambiar el estado de cada GPIO, a ‘0’ o a ‘1’, según el estado del bit 
 * correspondiente en el BCD ingresado. Ejemplo: b0 se encuentra en ‘1’, el estado de GPIO_20 debe setearse. 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 03/09/2025 | Document creation		                         |
 *
 * @author Lucia Faes (luchifaess@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct {
    gpio_t pin;   /*!< GPIO pin number */
    io_t dir;     /*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;
/*==================[internal functions declaration]=========================*/

// Función que escribe un dígito BCD en los GPIO
void bcdToGpio(uint8_t bcd_digit, gpioConf_t *pins) {
    for (int i = 0; i < 4; i++) {
        if((bcd_digit&(1<<i))==0){ //mascara para evaluar cada bit
            GPIOOff(pins[i].pin);
        }
        else GPIOOn(pins[i].pin);
        
    }
}

/*==================[external functions definition]==========================*/
void app_main(void){
   // Vector que mapea los bits del BCD a pines
    gpioConf_t bcdPins[4] = {
        {GPIO_20, GPIO_OUTPUT}, // b0
        {GPIO_21, GPIO_OUTPUT}, // b1
        {GPIO_22, GPIO_OUTPUT}, // b2
        {GPIO_23, GPIO_OUTPUT}  // b3
    };

    for (int i = 0; i < 4; i++) {
        GPIOInit(bcdPins[i].pin, bcdPins[i].dir);
    }

    bcdToGpio(6, bcdPins); //llama a la funcion dando el digito y el arreglo de pines
}
/*==================[end of file]============================================*/