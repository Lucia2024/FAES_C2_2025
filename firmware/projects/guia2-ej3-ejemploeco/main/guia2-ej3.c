/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * Cree un nuevo proyecto en el que modifi que la actividad del punto 2 agregando ahora el puerto serie. 
 * Envíe los datos de las mediciones para poder observarlos en un terminal en la PC, siguiendo el siguiente formato:
 * ● 3 dígitos ascii + 1 carácter espacio + dos caracteres para la unidad (cm) + cambio de línea “ \r\n”
 * Además debe ser posible controlar la EDU-ESP de la siguiente manera:
 * ● Con las teclas “O” y “H”, replicar la funcionalidad de las teclas 1 y 2 de la EDU-ESP
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
#include "uart_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

/*==================[internal functions declaration]=========================*/
void FuncUart(void* param);

/*==================[internal functions definition]==========================*/
void FuncUart(void* param){
    uint8_t caracter;
    // Leer un byte recibido
    UartReadByte(UART_PC, &caracter);
    caracter=caracter+1;
    // Reenviar el mismo byte (eco)
    UartSendByte(UART_PC,(char *)&caracter);
}

/*==================[external functions definition]==========================*/
void app_main(void){
    serial_config_t my_uart = {
        .port = UART_PC,
        .baud_rate = 9600,   // Velocidad de transmisión
        .func_p = FuncUart,  // Callback cuando llega un dato
        .param_p = NULL
    };

    // Inicializar UART con callback de recepción
    UartInit(&my_uart);
}

/*==================[end of file]============================================*/
