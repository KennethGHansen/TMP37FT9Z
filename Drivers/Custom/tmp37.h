#ifndef TMP37_H
#define TMP37_H

#include "stm32f4xx_hal.h"

/*
 * TMP37_Handle:
 *  - hadc : pointer to an ADC handle (e.g. &hadc1)
 *  - vref : ADC reference voltage (normally 3.3 V on Nucleo boards)
 */
typedef struct {
    ADC_HandleTypeDef *hadc;
    float vref;
} TMP37_Handle;

/*
 * Initializes the TMP37 driver.
 * For now this only stores the configuration — all hardware
 * initialization happens in your ADC and GPIO init functions.
 */
void TMP37_Init(TMP37_Handle *htmp);

/*
 * Reads the temperature using an Exponential Moving Average (EMA) filter.
 * The EMA smooths out ADC noise while staying responsive to changes.
 */
float TMP37_ReadFiltered(TMP37_Handle *htmp);

/*
 * Kalman Filter (1D)
 */

float TMP37_ReadFilteredKalman(TMP37_Handle *htmp);

#endif