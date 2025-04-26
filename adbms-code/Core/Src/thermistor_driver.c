#include "thermistor_driver.h"

// system prams
float SeriesResistance = 10000.0;
float R25 = 10000.0;
float B = 3435;
float T25 = 298.15;

float getTemp(float raw_temp_voltage, float Vref)
{
	// calc thermistor resistance
	float therm_resistance = (raw_temp_voltage * SeriesResistance) / (Vref - raw_temp_voltage);

	// calc temp from resistance
	float temp_kelvin = B / log(therm_resistance / (R25 * exp(-B / T25)));
	return temp_kelvin - 273.15; // Kelvin to Celsius
}
