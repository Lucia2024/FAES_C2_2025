/*! @mainpage guia1-ej3
 *
 * @section genDesc General Description
 *
 * Realice un función que reciba un puntero a una estructura LED como la que se muestra a continuación:
 *
 * struct leds {
 * uint8_t mode; ON, OFF, TOGGLE 
 * uint8_t n_led; indica el número de led a controlar 
 * uint8_t n_ciclos; indica la cantidad de ciclos de encendido/apagado 
 * uint16_t periodo; indica el tiempo de cada ciclo } my_leds;
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
 * | 20/08/2025 | Document creation		                         |
 *
 * @author Lucia Faes (luciafaess@gmail.com)
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
#define CONFIG_BLINK_PERIOD 100

/*==================[internal data definition]===============================*/
// Definimos constantes para los modos
#define LED_ON      0
#define LED_OFF     1
#define LED_TOGGLE  2

/*==================[external functions definition]==========================*/
// Definición de la estructura
typedef struct {
    uint8_t mode;       // ON, OFF, TOGGLE
    uint8_t n_led;      // LED number
    uint8_t n_ciclos;   // number of ON/OFF cycles
    uint16_t periodo;   // cycle time in ms
} leds_t;

// Función principal que recibe la estructura
void ledControl(leds_t * my_leds) {
    uint8_t i;
    
    switch(my_leds->mode) {
        case LED_ON:
            LedOn(my_leds->n_led);
            break;

        case LED_OFF:
            LedOff(my_leds->n_led);
            break;

        case LED_TOGGLE:
            for(i = 0; i < my_leds->n_ciclos; i++) {
                LedToggle(my_leds->n_led);          // cambio de estado
                vTaskDelay(my_leds->periodo /portTICK_PERIOD_MS); //o vTaskDelay(CONFIG_BLINK_PERIOD /portTICK_PERIOD_MS)
            }
            break;

        default:
            // Estado invalido
            break;
    }
}

void app_main(void) {
    LedsInit();
    leds_t my_leds;

    my_leds.mode = LED_TOGGLE;
    my_leds.n_led = LED_1;
    my_leds.n_ciclos = 10;    // 10 ciclos
    my_leds.periodo = 500;    // PERIODO ms

    ledControl(&my_leds);
}

/*==================[end of file]============================================*/