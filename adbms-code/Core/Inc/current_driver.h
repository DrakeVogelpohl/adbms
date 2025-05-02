#include "stm32f4xx_hal.h"
#include "ad_system_prams.h"

// Driver file for the current measuring. 
// The board takes a refrence voltage generated accross a shunt resistor
// and multiplies that by a fixed gain.

// Measures the current
float getCurrent(ADC_HandleTypeDef *hadc1);


// Gets an offset for current based on the startup value
float getCurrentOffset(ADC_HandleTypeDef *hadc1);
