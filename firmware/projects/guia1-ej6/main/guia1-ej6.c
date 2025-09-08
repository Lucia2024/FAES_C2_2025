/*! @mainpage Template
 *
 * @section genDesc General Description
 * 
 *Escriba una función que reciba un dato de 32 bits,  la cantidad de dígitos de salida
 *y dos vectores de estructuras del tipo  gpioConf_t. Uno  de estos vectores es igual al 
 *definido en el punto anterior y el otro vector mapea los puertos con el dígito del LCD 
 *a donde mostrar un dato:
 * Dígito 1 -> GPIO_19
 * Dígito 2 -> GPIO_18
 * Dígito 3 -> GPIO_9
 * La función deberá mostrar por display el valor que recibe. Reutilice las funciones 
 * creadas en el punto 4 y 5. Realice la documentación de este ejercicio usando Doxygen
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
 * | 03/09/2025 | Document creation		                         |
 *
 * @author Lucia Faes (luchifaess@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
/**
 * Ejercicio 6 - Mostrar un entero en la placa LCD (CD4543 + 3 selects)
 *
 * Reutiliza:
 *  - convertToBcdArray(...)  (ej. punto 4)
 *  - bcdToGpio(...)          (ej. punto 5)
 *
 * Se asume:
 *  - bcdPins[0] -> GPIO_20 = D1 (b0 LSB)
 *  - bcdPins[1] -> GPIO_21 = D2 (b1)
 *  - bcdPins[2] -> GPIO_22 = D3 (b2)
 *  - bcdPins[3] -> GPIO_23 = D4 (b3 MSB)
 *
 *  - selPins[0] -> GPIO_19 = SEL_DIGIT1  (digit 1, MSB position)
 *  - selPins[1] -> GPIO_18 = SEL_DIGIT2
 *  - selPins[2] -> GPIO_9  = SEL_DIGIT3  (digit 3, LSB position)
 */
#include "gpio_mcu.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*==================[macros and definitions]=================================*/

#define MAX_LCD_DIGITS 3

/*==================[internal data definition]===============================*/

typedef struct {
    gpio_t pin;
    io_t dir;
} gpioConf_t;

/*==================[internal functions declaration]=========================*/
/**
 * @brief Convierte un entero positivo en un array de dígitos decimales (MSB first).
 * @param data Valor a convertir (ej. 0..2^32-1)
 * @param digits Cantidad de dígitos que queremos (p.ej. 3 para display de 3 dígitos)
 * @param bcd_number Puntero a array con espacio para `digits` bytes.
 * @return  0 éxito, -1 error (digits==0 o puntero NULL)
 *
 * Ejemplo: data=123, digits=3 -> bcd_number[0]=1, [1]=2, [2]=3
 */
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number) {
    if (digits == 0 || bcd_number == NULL) return -1;
    // Rellenamos desde la derecha (LSB) y quedarán MSB-first en el arreglo.
    for (int i = digits - 1; i >= 0; i--) {
        bcd_number[i] = data % 10;
        data /= 10;
    }
    // Si data no cabe en 'digits', los dígitos más significativos contienen sólo (truncamiento).
    return 0;
}


/**
 * @brief Escribe el dígito BCD en los 4 pines (b0..b3 -> pins[0..3]).
 * @param bcd_digit valor 0..9 (se usan sólo 4 bits)
 * @param pins vector de 4 gpioConf_t (b0->pins[0], ...)
 */
void bcdToGpio(uint8_t bcd_digit, gpioConf_t *pins) {
    for (int i = 0; i < 4; i++) {
        bool bit = ((bcd_digit >> i) & 0x01) ? true : false;
        GPIOState(pins[i].pin, bit);
    }
}


/**
 * @brief Muestra un número en el display usando CD4543+SEL latch.
 * @param data número a mostrar (se mostrarán sólo los `digits` menos significativos si excede)
 * @param digits cantidad de dígitos a usar (<= MAX_LCD_DIGITS)
 * @param bcdPins vector de 4 gpioConf_t (D1..D4)
 * @param selPins vector de 'digits' gpioConf_t (SEL_1..SEL_n)
 * @return 0 éxito, -1 error de parámetros
 *
 * NOTA: La función efectúa un pulso en cada SEL para "latch" los datos.
 */
int8_t displayNumber(uint32_t data, uint8_t digits, gpioConf_t *bcdPins, gpioConf_t *selPins) {
    if (digits == 0 || digits > MAX_LCD_DIGITS || bcdPins == NULL || selPins == NULL) return -1;

    uint8_t bcd_array[MAX_LCD_DIGITS];
    if (convertToBcdArray(data, digits, bcd_array) != 0) return -1;

    for (int i = 0; i < digits; i++) {
        // Escribimos el dígito i (bcd_array[i]) en las líneas BCD
        bcdToGpio(bcd_array[i], bcdPins);

        // Pulso de latch en SEL correspondiente (CD4543 típico: pulso corto)
        GPIOOn(selPins[i].pin);                          // subimos SEL
        vTaskDelay(pdMS_TO_TICKS(1));                    // >= 1 ms es seguro (50 ns típico)
        GPIOOff(selPins[i].pin);                         // bajamos SEL

        // Pequeño retardo antes del siguiente dígito (no estrictamente necesario)
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    return 0;
}


/*==================[external functions definition]==========================*/
void app_main(void){
	    // Definir mapeo de pines BCD (D1..D4)
    gpioConf_t bcdPins[4] = {
        {GPIO_20, GPIO_OUTPUT}, // D1 <- b0 (LSB)
        {GPIO_21, GPIO_OUTPUT}, // D2 <- b1
        {GPIO_22, GPIO_OUTPUT}, // D3 <- b2
        {GPIO_23, GPIO_OUTPUT}  // D4 <- b3 (MSB)
    };

    // Definir mapeo de selects (Digit 1..3) según la guía
    gpioConf_t selPins[MAX_LCD_DIGITS] = {
        {GPIO_19, GPIO_OUTPUT},  // SEL digit 1 (MSB position)
        {GPIO_18, GPIO_OUTPUT},  // SEL digit 2
        {GPIO_9,  GPIO_OUTPUT}   // SEL digit 3 (LSB position)
    };

    // Inicializar todos los pines (BCD + SEL)
    for (int i = 0; i < 4; i++) {
        GPIOInit(bcdPins[i].pin, bcdPins[i].dir);
        // Opcional: dejar BCD en 0 al inicio
        GPIOOff(bcdPins[i].pin);
    }
    for (int i = 0; i < MAX_LCD_DIGITS; i++) {
        GPIOInit(selPins[i].pin, selPins[i].dir);
        GPIOOff(selPins[i].pin);
    }

    // Ejemplo: mostrar 123 en los 3 dígitos
    displayNumber(123, 3, bcdPins, selPins);

    // Si querés que se vea indefinidamente y no haya riesgo de que algo reseteé,
    // podés llamar displayNumber periódicamente (no es necesario si el latch
    // mantiene los valores).
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

}
/*==================[end of file]============================================*/