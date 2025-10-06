/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * Escriba una función que reciba un dato de 32 bits, 
 * la cantidad de dígitos de salida y un puntero a un arreglo donde 
 * se almacene los n dígitos. La función deberá convertir el dato 
 * recibido a BCD, guardando cada uno de los dígitos de salida en el 
 * arreglo pasado como puntero.
 * 
 * 
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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

int8_t convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
    // Paso 1: verificar que la cantidad de dígitos es suficiente
    uint32_t temp = data;
    uint8_t count = 0;

    while (temp > 0) {
        temp /= 10; //Descarta el bit menos significativo ya contado
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
        bcd_number[i] = data % 10; //tomo el ult dígito de data
        data /= 10; //corre el numero a la derecha
    }

    return 0; 
}

void app_main(void) {
    uint32_t numero = 12345;       // Número a convertir
    uint8_t digitos = 5;           // Cantidad de dígitos
    uint8_t bcd[digitos];          // Arreglo de salida
    int8_t resultado;

    resultado = convertToBcdArray(numero, digitos, bcd);

    if (resultado == 0) {
        printf("Número original: %lu\n", numero); //%lu muestra numeros de los arreglos
        printf("BCD (%d dígitos): ", digitos);
        for (int i = 0; i < digitos; i++) {
            printf("%d ", bcd[i]); //%d muestra nro de digitos
        }
        printf("\n");
    } else {
        printf("Error: el número no entra en %d dígitos\n", digitos);
    }

}
/*==================[end of file]============================================*/