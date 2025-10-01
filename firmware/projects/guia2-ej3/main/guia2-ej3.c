/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 *Cree un nuevo proyecto en el que modifi que la actividad del punto 2 agregando ahora el puerto serie. 
 *Envíe los datos de las mediciones para poder observarlos en un terminal en la PC, siguiendo el siguiente formato:
 *● 3 dígitos ascii + 1 carácter espacio + dos caracteres para la unidad (cm) + cambio de línea “ \r\n”
 *Además debe ser posible controlar la EDU-ESP de la siguiente manera:
 *● Con las teclas “O” y “H”, replicar la funcionalidad de las teclas 1 y 2 de la EDU-ESP
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
 * | 1/10/2025 | Document creation		                         |
 *
 * @author Faes Lucia (luchifaess@gmail.com)
 *
 */
/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "led.h"
#include "switch.h"
#include "lcditse0803.h"
#include "hc_sr04.h"
#include "uart_mcu.h"

/*==================[macros and definitions]=================================*/
#define MEDICION_PERIOD_US   1000000   /**< Período de medición: 1 segundo (en us) */
#define UART_BAUDRATE        115200    /**< Velocidad de transmisión UART */

/*==================[internal data definition]===============================*/
volatile bool medir = false;     /**< Flag: habilita/inhibe la medición */
volatile bool hold = false;      /**< Flag: congela el resultado mostrado */
uint16_t distancia = 0;          /**< Última distancia medida en cm */
TaskHandle_t medir_task_handle = NULL; /**< Handle de la tarea de medición */

/*==================[internal functions declaration]=========================*/
void CallbackTec1(void *args);
void CallbackTec2(void *args);
void FuncTimerMedicion(void* param);
static void MedirDistanciaTask(void *pvParameter);
static void UartTask(void *pvParameter);

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

        // Solo mide si está habilitado y no en hold
        if(medir && !hold){
            distancia = HcSr04ReadDistanceInCentimeters();
            LcdItsE0803Write(distancia); // Muestra valor en display 7 segmentos
        }
    }
}

/**
 * @brief Tarea que envía la distancia por UART y recibe comandos desde la PC
 */
static void UartTask(void *pvParameter){
    char msg[32];
    uint8_t rxByte;

    while(true){
        // ---- Envío de distancia ----
        if(medir && !hold){
            snprintf(msg, sizeof(msg), "%03d cm\r\n", distancia);
            UartSendString(UART_PC, msg);
        }

        // ---- Recepción de comandos ----
        if(UartReadByte(UART_PC, &rxByte)){
            if(rxByte == 'O' || rxByte == 'o'){   // ON/OFF
                medir = !medir;
            } else if(rxByte == 'H' || rxByte == 'h'){ // HOLD
                hold = !hold;
            }
        }

        vTaskDelay(200 / portTICK_PERIOD_MS); // Refresco de UART
    }
}

/*==================[external functions definition]==========================*/
void app_main(void){
    // Inicialización de periféricos
    LedsInit();
    SwitchesInit();
    LcdItsE0803Init();
    HcSr04Init(GPIO_3, GPIO_2);   // ECHO=GPIO_3, TRIG=GPIO_2

    // Configuración UART
    serial_config_t uart_pc = {
        .port = UART_PC,
        .baud_rate = UART_BAUDRATE,
        .func_p = UART_NO_INT,   // Sin interrupción de recepción, polling
        .param_p = NULL
    };
    UartInit(&uart_pc);

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
    xTaskCreate(UartTask,          "UartTask",  2048, NULL, 5, NULL);

    // Arranque del timer
    TimerStart(timer_medicion.timer);
}

/*==================[end of file]============================================*/
