/*! @mainpage Guia 2. Ejercicio 4
 *
 * @section genDesc General Description
 *
 * Diseñar e implementar una aplicación, basada en el driver analog io mcu.y el driver de transmisión 
 * serie uart mcu.h, que digitalice una señal analógica y la transmita a un grafi cador de puerto serie
 * de la PC. Se debe tomar la entrada CH1 del conversor AD y la transmisión se debe realizar por la UART 
 * conectada al puerto serie de la PC, en un formato compatible con un grafi cador por puerto serie.
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
 * @author Faes Lucia (luchifaess@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

void app_main(void)
{
    // Configurar UART hacia la PC
    serial_config_t config_uart = {
        .port = UART_PC,
        .baud_rate = 115200,
        .func_p = UART_NO_INT,
        .param_p = NULL
    };
    UartInit(&config_uart);

    //  Variables
    uint16_t valor = 0;
    char mensaje[50];     // buffer para armar la cadena
    char *valor_str;      // puntero al texto convertido

    // Bucle principal
    while (1)
    {
        // Convertir número a string decimal
        valor_str = (char *)UartItoa(valor, 10);

        // Construir mensaje
        strcpy(mensaje, ">brightness:");
        strcat(mensaje, valor_str);
        strcat(mensaje, "\r\n");

        // Enviar por UART
        UartSendString(UART_PC, mensaje);

        // Incrementar valor (onda de diente de sierra)
        valor = (valor + 10) % 255;

        vTaskDelay(pdMS_TO_TICKS(100)); // cada 100 ms
    }
}

/*==================[end of file]============================================*/