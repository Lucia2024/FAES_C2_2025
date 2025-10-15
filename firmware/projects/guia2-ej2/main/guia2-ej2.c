/*! @mainpage Guia 2. Ejercicio 2
 *
 * @section genDesc General Description
 *
 * Cree un nuevo proyecto en el que modifique la actividad del punto 1 de manera de utilizar interrupciones 
 * para el control de las teclas y el control de tiempos (Timers)
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	GPIO_3		|
 * |    TRIGGER     |   GPIO_2      |
 * |    +5V         |   +5V         |
 * |    GND         |   GND         |
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 24/09/2025 | Document creation		                         |
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
#include "timer_mcu.h"
#include "led.h"
#include "switch.h"
#include "lcditse0803.h"
#include "hc_sr04.h"

/*==================[macros and definitions]=================================*/
#define MEDICION_PERIOD_US   1000000   /**< Período de medición: 1 segundo */

/*==================[internal data definition]===============================*/
volatile bool medir = false;     /**< Flag: habilita/inhibe la medición */
volatile bool hold = false;      /**< Flag: congela el resultado mostrado */
uint16_t distancia = 0;          /**< Última distancia medida en cm */
TaskHandle_t medir_task_handle = NULL; /**< Handle de la tarea de medición */

/*==================[internal functions declaration]=========================*/
/**
 * @brief Callback asociado a la tecla TEC1 (SWITCH_1).
 * Alterna el estado de la bandera "medir".
 */
void CallbackTec1(void *args);

/**
 * @brief Callback asociado a la tecla TEC2 (SWITCH_2).
 * Alterna el estado de la bandera "hold".
 */
void CallbackTec2(void *args);

/**
 * @brief Callback asociado al timer de medición.
 * Notifica a la tarea de medición para que ejecute la lectura.
 */
void FuncTimerMedicion(void* param);

/**
 * @brief Tarea encargada de realizar la medición con el sensor
 * ultrasónico +controlar leds + mostrar en el display LCD.
 */
static void MedirDistanciaTask(void *pvParameter);


/*==================[internal functions definition]==========================*/
void CallbackTec1(void *args){
    medir = !medir;   // Activa o detiene la medición
	LedOn(LED_1);
}

void CallbackTec2(void *args){
    hold = !hold;     // Congela o libera el resultado
	LedOn(LED_2);
}

void FuncTimerMedicion(void* param){
    // Envía notificación a la tarea de medición (desde ISR)
    vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);
}

static void MedirDistanciaTask(void *pvParameter){
    while(true){
        // Espera hasta recibir notificación del timer
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(medir){  // Si la medición está activada
            distancia = HcSr04ReadDistanceInCentimeters();

            if(!hold){   // Si no está en HOLD
                LcdItsE0803Write(distancia);
            }
            // Control de LEDs según la distancia
            if(distancia < 10){
                LedsOffAll();
            }
            else if(distancia >= 10 && distancia < 20){
                LedOn(LED_1);
                LedOff(LED_2);
                LedOff(LED_3);
            }
            else if(distancia >= 20 && distancia < 30){
                LedOn(LED_1);
                LedOn(LED_2);
                LedOff(LED_3);
            }
            else if(distancia >= 30){
                LedOn(LED_1);
                LedOn(LED_2);
                LedOn(LED_3);
            }
        }else{
            // Si no está activa la medición
            LcdItsE0803Off();
            LedsOffAll();
        }
    }
}


/*==================[external functions definition]==========================*/
void app_main(void){
    // Inicialización de periféricos
    LedsInit();
    SwitchesInit();
    LcdItsE0803Init();
    HcSr04Init(GPIO_3, GPIO_2);   // ECHO=GPIO_3, TRIG=GPIO_2

    // Configuración de interrupciones de teclas
    SwitchActivInt(SWITCH_1, CallbackTec1, NULL);
    SwitchActivInt(SWITCH_2, CallbackTec2, NULL);

    // Configuración de timer para medición periódica
    timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = MEDICION_PERIOD_US,
        .func_p = FuncTimerMedicion,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);

    // Creación de tareas
    xTaskCreate(MedirDistanciaTask, "MedirDist", 2048, NULL, 5, &medir_task_handle);

    // Arranque del timer
    TimerStart(timer_medicion.timer);
}

/*==================[end of file]============================================*/
