#ifndef AD_SYSTEM_PARAMS_H
#define AD_SYSTEM_PARAMS_H

// Note for NFR25 we precharged across the negative contactor. 
// This might change in future years, if so change which contactors are closed accoringly in the FSM 

#define NUM_CHIPS 10					/* Number of ICs 					*/
#define NUM_VOLTAGES_CHIP 14			/* Number of Cells to read per IC 	*/
#define NUM_TEMPS_CHIP 8

#define OVERVOLTAGE 4.2					/* Overvoltage Threshold 	*/
#define UNDERVOLTAGE 2.5				/* Undervoltage Threshold 	*/
#define OVERTEMP 90						/* Over Temp Threshold 		*/
#define UNDERTEMP -40					/* Under Temp Threshold 	*/
#define OVERCURRENT 135					/* Over Current Threshold 	*/

#define PEC_FAILURE_THRESHOLD 10		/* Number of consecutive PEC failures that can happen before a pec fault occurs*/

#define CB_THRESHOLD 0.01				/* Threshold Away From Lowest Cell to Start Cell Balancing when CB is enabled */
#define CB_MIN_V_THRESHOLD 3			/* Cells under this threshold will not be balanced even when CB is enabled */

#define ENABLE_PRINTF_DEBUG_COMMS 0		/* Flag to enable Printf debug comms */
#define ENABLE_USB_COMMS 1				/* Flag to enable USB comms */

#define NUM_CURRENT_OFFSET_CYCLES 10	/* Numbeer of Cycles to get ADC Offset */

#define NUM_DATA_CAN_VOLTAGES_PER_MSG 7 // will break if this is changed. Based on how data can and DBC are set up
#define NUM_DATA_CAN_VOLTAGE_MSGS (NUM_CHIPS * NUM_VOLTAGES_CHIP) / NUM_DATA_CAN_VOLTAGES_PER_MSG
#define NUM_DATA_CAN_TEMPS_PER_MSG 8    // will break if this is changed. Based on how data can and DBC are set up
#define NUM_DATA_CAN_TEMP_MSGS (NUM_CHIPS * NUM_TEMPS_CHIP) / NUM_DATA_CAN_TEMPS_PER_MSG


// ENUMERATES
enum fsm_states
{
	Idle = 0,
	Precharge = 1,
	Active = 2,
	Charge = 3,
	Fault = 4
};

#endif // AD_SYSTEM_PARAMS_H
