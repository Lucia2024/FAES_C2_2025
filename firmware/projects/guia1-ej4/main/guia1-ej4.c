/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * Escriba una función que reciba un dato de 32 bits, 
 * la cantidad de dígitos de salida y un puntero a un arreglo donde 
 * se almacene los n dígitos. La función deberá convertir el dato 
 * recibido a BCD, guardando cada uno de los dígitos de salida en el 
 * arreglo pasado como puntero.
 * Utilizamos punteros porque en C no podemos retornar mas de una variable con una funcion
 * 
 * <a href="https://drive.google.com/...">Operation Example</a>
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
 * | 21/08/2025 | Document creation		                         |
 *
 * @author Lucia Faes (luciafaess@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
/**
 * @brief Convierte un número entero en sus dígitos BCD.
 *
 * @param data: número entero de 32 bits a convertir (ejemplo: 12345).
 * @param digits: cantidad de dígitos que se desean en la salida (ej: 5).
 * @param bcd_number: puntero a un arreglo donde se guardarán los dígitos.
 *
 * @return 0 si todo salió bien.
 *        -1 si hubo un error (ejemplo: el número no entra en la cantidad de dígitos).
 */

int8_t convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number);

/*==================[external functions definition]==========================*/

int8_t convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
    // Paso 1: verificar que la cantidad de dígitos es suficiente
    uint32_t temp = data;
    uint8_t count = 0;

    while (temp > 0) {
        temp /= 10;
        count++;
    }
    /* Si el número es 0 contamos 1 dígito */
    if (data == 0) {
        count = 1;
    }

    if (count > digits) {
        return -1; // Error: no entran los dígitos en el arreglo
    }

    // Paso 2: inicializar el arreglo en 0
    for (int i = 0; i < digits; i++) {
        bcd_number[i] = 0;
    }

    // Paso 3: extraer dígitos desde el menos significativo
    for (int i = digits - 1; i >= 0; i--) {
        bcd_number[i] = data % 10;
        data /= 10;
    }

    return 0; // Éxito
}

void app_main(void) {
    uint32_t numero = 12345;       // Número a convertir
    uint8_t digitos = 6;           // Cantidad de dígitos
    uint8_t bcd[6];                // Arreglo de salida
    int8_t resultado;

    resultado = convertToBcdArray(numero, digitos, bcd);

    if (resultado == 0) {
        printf("Número original: %lu\n", numero);
        printf("BCD (%d dígitos): ", digitos);
        for (int i = 0; i < digitos; i++) {
            printf("%d ", bcd[i]);
        }
        printf("\n");
    } else {
        printf("Error: el número no entra en %d dígitos\n", digitos);
    }

}
/*==================[end of file]============================================*/