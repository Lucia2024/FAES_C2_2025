/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * Actividad 1: Diseñar el firmware modelando con un diagrama de flujo de manera que cumpla 
 * con las siguientes funcionalidades:
 * Mostrar distancia medida utilizando los leds de la siguiente manera:
 * -Si la distancia es menor a 10 cm, apagar todos los LEDs.
 * -Si la distancia está entre 10 y 20 cm, encender el LED_1.
 * -Si la distancia está entre 20 y 30 cm, encender el LED_2 y LED_1.
 * -Si la distancia es mayor a 30 cm, encender el LED_3, LED_2 y LED_1.
 * 
 * Mostrar el valor de distancia en cm utilizando el display LCD.
 * Usar TEC1 para activar y detener la medición.
 * Usar TEC2 para mantener el resultado (“HOLD”).
 * Refresco de medición: 1 s
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 10/09/2025 | Document creation		                         |
 *
 * @author Faes Lucia (luchiifaes@gmail.com)
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
#include "lcditse0803.h"
#include "hc_sr04.h"

/*==================[macros and definitions]=================================*/
#define REFRESH_TIME_MS   1000   // 1 segundo

/*==================[internal data definition]===============================*/
volatile bool medir = false;   // TEC1 activa/desactiva
volatile bool hold = false;    // TEC2 congela la medición
uint16_t distancia = 0;        // última distancia medida (en cm)

/*==================[tarea 1: medir distancia]===============*/
static void MedirDistanciaTask(void *pvParameter){
    while(true){
        if(medir && !hold){
            // medir distancia
            distancia = HcSr04ReadDistanceInCentimeters();

            // mostrar en LCD (solo valores numéricos 0..999)
            LcdItsE0803Write(distancia);
        }
        vTaskDelay(REFRESH_TIME_MS / portTICK_PERIOD_MS);
    }
}

/*==================[tarea 2: controlar LEDs]================*/
static void ControlLedsTask(void *pvParameter){
    while(true){
        if(medir){
            if(!hold){
                // actualizar LEDs según la distancia
                if(distancia < 10){
                    LedOff(LED_1);
                    LedOff(LED_2);
                    LedOff(LED_3);
                } else if(distancia < 20){
                    LedOn(LED_1);
                    LedOff(LED_2);
                    LedOff(LED_3);
                } else if(distancia < 30){
                    LedOn(LED_1);
                    LedOn(LED_2);
                    LedOff(LED_3);
                } else {
                    LedOn(LED_1);
                    LedOn(LED_2);
                    LedOn(LED_3);
                }
            }
        } else {
            // medición detenida → apagar todos los LEDs
            LedOff(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/*==================[tarea 3: controlar teclas]==============*/
static void ControlTeclasTask(void *pvParameter){
    while(true){
        int8_t teclas = SwitchesRead();
        if(teclas & SWITCH_1){
            medir = !medir;   // arranca o detiene medición
            vTaskDelay(300 / portTICK_PERIOD_MS); // antirrebote
        }
        if(teclas & SWITCH_2){
            hold = !hold;     // congela o libera pantalla/leds
            vTaskDelay(300 / portTICK_PERIOD_MS); // antirrebote
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/*==================[external functions definition]==========================*/
void app_main(void){
    // Inicialización de drivers
    LedsInit();
    SwitchesInit();
    LcdItsE0803Init();
    HcSr04Init(GPIO_3, GPIO_2);   // ECHO = GPIO_3, TRIG = GPIO_2

    // Crear las tres tareas del diagrama
    xTaskCreate(&MedirDistanciaTask, "MedirDist", 2048, NULL, 5, NULL);
    xTaskCreate(&ControlLedsTask,   "CtrlLeds",   1024, NULL, 5, NULL);
    xTaskCreate(&ControlTeclasTask, "CtrlKeys",   1024, NULL, 5, NULL);
}
