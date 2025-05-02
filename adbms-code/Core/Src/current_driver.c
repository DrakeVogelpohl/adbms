#include "current_driver.h"

float getCurrent(ADC_HandleTypeDef *hadc)
{
    // Start ADC and poll it
	HAL_ADC_Start(hadc);
	HAL_StatusTypeDef adc_poll_status = HAL_ADC_PollForConversion(hadc, 5000);

    // lazy handling of adc polling
    if(adc_poll_status != HAL_OK){
    	HAL_ADC_Stop(hadc);
    	printf("current is not OK\n");
        return -1;
    }

    // take 12 bit adc and convert into volts
    float raw_current = ((float) HAL_ADC_GetValue(hadc));
	float current_adc_voltage = raw_current*3.3/4095;

	// i = v/r
	// Offset of 1.25V so can read both positive and negative current
	// where negative current is charging and positive is discharging
	// (adc_voltage - 1.65V) / (diff-op-amp gain of 2 * iso amp fixed gain of 41 (AMC3302DWE))
	// shunt resistance = 100u Ohms (SH6918F500BHEP)
	float current_adc_offset = current_adc_voltage - 1.25;
	float current = current_adc_offset/(2*41*0.0001);

    // Stop ADC
	HAL_ADC_Stop(hadc);
    return current;
}


float getCurrentOffset(ADC_HandleTypeDef *hadc)
{
	float total_current = 0;
	for(int i = 0; i < NUM_CURRENT_OFFSET_CYCLES; i++){
		total_current += getCurrent(hadc);
	}
	return total_current / NUM_CURRENT_OFFSET_CYCLES;
}
