#include "tmp37.h"

/*
 * Initializes the TMP37 handle.
 * No hardware configuration is needed here because:
 *  - TMP37 is a purely analog device
 *  - ADC and GPIO are initialized in MX_ADC1_Init() and MX_GPIO_Init()
 */
void TMP37_Init(TMP37_Handle *htmp)
{
    // Nothing to do, but provided for API completeness.
    (void)htmp;
}

/*
 * Reads temperature from TMP37 and applies an Exponential Moving Average filter.
 *
 * TMP37 Datasheet:
 *  - Output voltage (Vout) = Temperature(°C) * 20 mV
 *  - No offset (0°C = 0V)
 *
 * Conversion:
 *  1) raw ADC reading -> voltage
 *     V = raw / 4095 * Vref
 *
 *  2) voltage -> temperature
 *     T = Vout / 0.02
 *
 * EMA Filter:
 *     filtered = filtered + alpha * (new - filtered)
 */
float TMP37_ReadFiltered(TMP37_Handle *htmp)
{
    static float filteredTemp = 0.0f;    // holds previous filtered value
    static uint8_t initialized = 0;      // tracks first run

    // 1. Start ADC conversion
    HAL_ADC_Start(htmp->hadc);

    // 2. Wait for conversion to complete (blocking, safe for slow sampling)
    HAL_ADC_PollForConversion(htmp->hadc, HAL_MAX_DELAY);

    // 3. Read raw ADC value (0–4095)
    uint32_t raw = HAL_ADC_GetValue(htmp->hadc);

    // 4. Convert raw ADC value to sensor output voltage
    float vout = ((float)raw / 4095.0f) * htmp->vref;

    // 5. Convert voltage to temperature (TMP37 has no offset)
    float tempC = vout / 0.02f;  // 0.02V = 20mV per °C

    // 6. Initialize EMA filter on first run
    if (!initialized) {
        filteredTemp = tempC;
        initialized = 1;
    }

    // 7. EMA smoothing factor (adjust as needed: 0.1 = very smooth, 0.25 = responsive)
    float alpha = 0.01f;

    // 8. EMA update
    filteredTemp += alpha * (tempC - filteredTemp);

    // 9. Return the filtered temperature
    return filteredTemp;
}

/*
 * ------------- Kalman Filter Theory (1D) --------------
 *
 * We estimate one variable: temperature.
 *
 * State variable:
 *   x = estimated temperature (°C)
 *
 * State uncertainty:
 *   P = estimated error covariance
 *
 * Measurement:
 *   z = measured temperature (from ADC)
 *
 * Two tunable constants:
 *   Q = process noise       (how much we allow the temperature to drift/change)
 *   R = measurement noise   (how noisy the ADC readings are)
 *
 * Typical values for temperature sensors:
 *   Q = 0.01 to 0.1
 *   R = 1.0 to 5.0
 *
 * Filter steps:
 *
 *  Predict:
 *     x = x
 *     P = P + Q
 *
 *  Update:
 *     K = P / (P + R)
 *     x = x + K * (z - x)
 *     P = (1 - K) * P
 */

float TMP37_ReadFilteredKalman(TMP37_Handle *htmp)
{
    // ----- Kalman filter static state -----
    static uint8_t initialized = 0;
    static float x = 0.0f;   // estimated temperature
    static float P = 1.0f;   // estimated error covariance

    // Tunable parameters:
	  /*
	  Q (process noise) represents how much you expect the actual temperature to randomly change between measurements.
    Low Q = “temperature changes slowly,” so the filter behaves like a heavy damper.
    R (measurement noise) represents how noisy your ADC readings are.
    High R = “measurements are noisy, trust them less,” so the filter smooths more aggressively.
		If your goal is to prioritize smoothness more than responsiveness:
		Q = 0.001 – 0.01
		Extremely low → assumes temperature changes are slow and predictable.
		R = 3 – 10
		Higher measurement noise value → reduces the influence of noisy ADC samples.
		This combination causes the Kalman filter to:
    - Smooth out even small noise fluctuations
    - Produce a very stable output
    - Change only slowly when real temperature changes occur
	  */
	
	  float Q = 0.001f;   // process noise (smooth vs fast response)
    float R = 5.0f;    // measurement noise (ADC noise level)

    // ---- Step 1: Read raw ADC ----
    HAL_ADC_Start(htmp->hadc);
    HAL_ADC_PollForConversion(htmp->hadc, HAL_MAX_DELAY);

    uint32_t raw = HAL_ADC_GetValue(htmp->hadc);

    // Convert raw ADC to voltage
    float vout = ((float)raw / 4095.0f) * htmp->vref;

    // Convert TMP37 voltage to temperature
    float z = vout / 0.02f;   // measured temperature

    // ---- Initialize filter on first run ----
    if (!initialized) {
        x = z;         // set estimate to first measurement
        P = 1.0f;      // reasonable initial uncertainty
        initialized = 1;
        return x;
    }

    // ------------- Kalman Predict Step -------------
    // Predict new state (temperature doesn't change without measurement)
    x = x;

    // Increase uncertainty by process noise
    P = P + Q;

    // ------------- Kalman Update Step -------------
    // Compute Kalman gain
    float K = P / (P + R);

    // Update estimate with measurement z
    x = x + K * (z - x);

    // Update uncertainty
    P = (1.0f - K) * P;

    // The result x is the smooth filtered temperature
    return x;
}
