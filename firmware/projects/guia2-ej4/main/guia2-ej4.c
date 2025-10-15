/*! @mainpage Guia 2. Ejercicio 4
 *
 * @section genDesc General Description
 *
 * Diseñar e implementar una aplicación, basada en el driver analog io mcu.y el driver de transmisión serie 
 * uart mcu.h, que digitalice una señal analógica y la transmita a un grafi cador de puerto serie de la PC. 
 * Se debe tomar la entrada CH1 del conversor AD y la transmisión se debe realizar por la UART conectada al 
 * puerto serie de la PC, en un formato compatible con un grafi cador por puerto serie.
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
 * | 8/10/2025 | Document creation		                         |
 *
 * @author Faes Lucia (luchifaes@gmail.com)
 *
 */
//*==================[inclusions]=============================================*/ 
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"

/*==================[macros]=================================*/
#define SAMPLE_FREQ_HZ    500                     // Frecuencia de muestreo
#define TIMER_PERIOD_US2   (1000000 / SAMPLE_FREQ_HZ)
#define TIMER_PERIOD_US   2000
#define UART_BAUDRATE      115200
#define BUFFER_SIZE 231
/*==================[variables globales]====================*/
TaskHandle_t adc_task_handle = NULL;
TaskHandle_t ecg_task_handle = NULL;
uint8_t dato = 0;
/*==================[ECG data - ejemplo]========================*/
const char ecg_signal[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};

/*==================[handlers]==============================*/
/**
 * @brief Handler del timer: notifica a las tareas ADC y ECG.
 */
void TimerHandler(void *param){
    vTaskNotifyGiveFromISR(adc_task_handle, pdFALSE);
    vTaskNotifyGiveFromISR(ecg_task_handle, pdFALSE);
}

/*==================[tareas]================================*/
/**
 * @brief Tarea que lee el ADC y envía los datos por UART.
 */
void AdcTask(void *pvParameter){
    uint16_t adc_val_mV;
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
       
        // Leer entrada analógica (en mV)
        AnalogInputReadSingle(CH1, &adc_val_mV);

        // Enviar formato compatible con Serial Plotter
        UartSendString(UART_PC, (char*)UartItoa(adc_val_mV, 10));
        UartSendString(UART_PC, (char*)"\r\n");
    }
}

/**
 * @brief Tarea que simula la generación de la señal ECG usando salida analógica.
 */
static void EcgTask(void *pvParameter){
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        AnalogOutputWrite(ecg_signal[dato]); // Convierte de digital a analógico
        dato ++;
        
        if (dato == sizeof(ecg_signal)) // Recorro los datos del ECG
            dato = 0;
    }
}



/*==================[función principal]======================*/
void app_main(void){

    /* --- Inicialización UART --- */
    serial_config_t my_uart = {
        .port = UART_PC,
        .baud_rate = UART_BAUDRATE,
        .func_p = UART_NO_INT,   // no se usa RX
        .param_p = NULL
    };
    UartInit(&my_uart);

    /* --- Inicialización ADC --- */
    analog_input_config_t Aanalog_imput1 = {
        .input = CH1,
    };
    AnalogInputInit(&Aanalog_imput1);

    /* --- Inicialización salida analógica (simulación ECG) --- */
    AnalogOutputInit();

    /* --- Configuración de los timers --- */
    timer_config_t timer_adc = {
        .timer = TIMER_A,
        .period = TIMER_PERIOD_US,
        .func_p = TimerHandler,
        .param_p = NULL
    };
    TimerInit(&timer_adc);

    timer_config_t timer_ecg = {
        .timer = TIMER_A,
        .period = TIMER_PERIOD_US2,
        .func_p = TimerHandler,
        .param_p = NULL
    };
    TimerInit(&timer_adc);

    /* --- Crear tareas --- */
    xTaskCreate(&AdcTask, "AdcTask", 2048, NULL, 5, &adc_task_handle);
    xTaskCreate(&EcgTask, "EcgTask", 2048, NULL, 5, &ecg_task_handle);

    /* --- Iniciar timer --- */
    TimerStart(timer_adc.timer);
    TimerStart(timer_ecg.timer);
}
