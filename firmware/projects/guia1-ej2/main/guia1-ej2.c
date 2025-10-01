/*! @mainpage Blinking switch
 *
 * \section genDesc General Description
 *
 * Modifique la aplicación 1_blinking_switch de manera de hacer titilar los leds 1 y 2 al mantener 
 * presionada las teclas 1 y 2 correspondientemente. También se debe poder hacer titilar el led 3 al 
 * presionar simultáneamente las teclas 1 y 2.
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 20/08/2025 | Document creation		                         |
 *
 * @author Faes Lucia (luchifaess@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"

/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

void app_main(void){
	uint8_t teclas;
	LedsInit();
	SwitchesInit();
    while(1)    {
    	teclas  = SwitchesRead();
    	switch(teclas){
    		case SWITCH_1:
    			LedToggle(LED_1); //toggle cambia de estado
    		break;
    		case SWITCH_2:
    			LedToggle(LED_2);
    		break;
			case SWITCH_1 | SWITCH_2:
			    LedToggle(LED_3);
				break;
    	}
	    
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}
