#include "bms_can.h"

adbms_can_ adbms_can;

void ADBMS_Initialize_Can(mainboard_ *mainboard)
{
	// Start CAN
	adbms_can.mainboard = mainboard;
	HAL_CAN_Start(adbms_can.mainboard->hcan_drive);
	HAL_CAN_Start(adbms_can.mainboard->hcan_data);

	// Enable notifications (interrupts) for CAN, uses FIFO scheduling to receive msgs
	HAL_CAN_ActivateNotification(adbms_can.mainboard->hcan_drive, CAN_IT_RX_FIFO0_MSG_PENDING);
	// HAL_CAN_ActivateNotification(adbms_can.mainboard->hcan_data, CAN_IT_RX_FIFO0_MSG_PENDING);	// Don't need to read Data CAN

	// SOC header initialization
	adbms_can.TxHeaderSOC_.StdId = 0x150;
	adbms_can.TxHeaderSOC_.IDE = CAN_ID_STD;
	adbms_can.TxHeaderSOC_.RTR = CAN_RTR_DATA;
	adbms_can.TxHeaderSOC_.DLC = 8; // we're sending 8 bytes of data for SOE

	// Faults header initialization
	adbms_can.TxHeaderFaults_.StdId = 0x151;
	adbms_can.TxHeaderFaults_.IDE = CAN_ID_STD;
	adbms_can.TxHeaderFaults_.RTR = CAN_RTR_DATA;
	adbms_can.TxHeaderFaults_.DLC = 8;

	// Status header initialization
	adbms_can.TxHeaderStatus_.StdId = 0x152;
	adbms_can.TxHeaderStatus_.IDE = CAN_ID_STD;
	adbms_can.TxHeaderStatus_.RTR = CAN_RTR_DATA;
	adbms_can.TxHeaderStatus_.DLC = 8;

	// Charger header initialization
	adbms_can.TxHeaderCharger_.ExtId = 0x1806E5F4;
	adbms_can.TxHeaderCharger_.IDE = CAN_ID_EXT;
	adbms_can.TxHeaderCharger_.RTR = CAN_RTR_DATA;
	adbms_can.TxHeaderCharger_.DLC = 8;

	// Voltages header initialization
	adbms_can.TxHeaderVoltages_.StdId = 0x153;
	adbms_can.TxHeaderVoltages_.IDE = CAN_ID_STD;
	adbms_can.TxHeaderVoltages_.RTR = CAN_RTR_DATA;
	adbms_can.TxHeaderVoltages_.DLC = 8;

	// Temperatures header initialization
	adbms_can.TxHeaderTemperatures_.StdId = 0x167;
	adbms_can.TxHeaderTemperatures_.IDE = CAN_ID_STD;
	adbms_can.TxHeaderTemperatures_.RTR = CAN_RTR_DATA;
	adbms_can.TxHeaderTemperatures_.DLC = 8;
}

void drive_can_loop()
{
	// printf("Sending Drive CAN\n");

	// update and send soc
	populateBMS_SOC(adbms_can.txDataSOC_);
	send_can_messages(adbms_can.mainboard->hcan_drive, &adbms_can.TxHeaderSOC_, adbms_can.txDataSOC_, &adbms_can.TxMailBox_);

	// update and send faults
	populateBMS_Faults(adbms_can.txDataFaults_);
	send_can_messages(adbms_can.mainboard->hcan_drive, &adbms_can.TxHeaderFaults_, adbms_can.txDataFaults_, &adbms_can.TxMailBox_);

	// update and send status
	populateBMS_Status(adbms_can.txDataStatus_);
	send_can_messages(adbms_can.mainboard->hcan_drive, &adbms_can.TxHeaderStatus_, adbms_can.txDataStatus_, &adbms_can.TxMailBox_);
}

void data_can_loop()
{
	// printf("Sending Data CAN\n");

	// send voltage messages
	adbms_can.TxHeaderVoltages_.StdId = 0x153; // set the message id for next iteration
	for(int i = 0; i < NUM_DATA_CAN_VOLTAGE_MSGS; i++) {
		populateBMS_VoltageMessages(adbms_can.txDataVoltages_, i);
		send_can_messages(adbms_can.mainboard->hcan_data, &adbms_can.TxHeaderVoltages_, adbms_can.txDataVoltages_, &adbms_can.TxMailBox_);
		adbms_can.TxHeaderVoltages_.StdId++;
	}

	// send temperature messages
	adbms_can.TxHeaderTemperatures_.StdId = 0x167; // set the message id for next iteration
	for(int i = 0; i < NUM_DATA_CAN_TEMP_MSGS; i++) {
		populateBMS_TemperatureMessages(adbms_can.txDataTemperatures_, i);
		send_can_messages(adbms_can.mainboard->hcan_data, &adbms_can.TxHeaderTemperatures_, adbms_can.txDataTemperatures_, &adbms_can.TxMailBox_);
		adbms_can.TxHeaderTemperatures_.StdId++;
	}
}

uint8_t send_can_messages(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *TxHeader, uint8_t *data, uint32_t *TxMailBox)
{
	// send msg
	HAL_StatusTypeDef msg_status = HAL_CAN_AddTxMessage(hcan, TxHeader, data, TxMailBox);

	if (msg_status != HAL_OK)
	{
		// Error handling
		printf("CAN Message failed\n");
		return 1;
	}
	return 0;
}

void populateBMS_SOC(uint8_t *data)
{
	RawCanSignal signals[5];
	populateRawMessage(&signals[0], 0, 12, 0.1, 0);									  // max discharge current
	populateRawMessage(&signals[1], 0, 12, 0.1, 0);									  // max regen current
	populateRawMessage(&signals[2], adbms_can.mainboard->adbms.total_v, 16, 0.01, 0); // battery voltage
	populateRawMessage(&signals[3], adbms_can.mainboard->adbms.avg_temp, 8, 1, -40);  // battery temp
	populateRawMessage(&signals[4], adbms_can.mainboard->current, 16, 0.01, 0);		  // battery current
	encodeSignals(data, 5, signals[0], signals[1], signals[2], signals[3], signals[4]);
}

void populateBMS_Faults(uint8_t *data)
{
	RawCanSignal signals[8];
	populateRawMessage(&signals[0], adbms_can.mainboard->bms_fault, 1, 1, 0);																  	// fault summary
	populateRawMessage(&signals[1], adbms_can.mainboard->adbms.undervoltage_fault_, 1, 1, 0);												  	// undervoltage fault
	populateRawMessage(&signals[2], adbms_can.mainboard->adbms.overvoltage_fault_, 1, 1, 0);												   	// overvoltage fault
	populateRawMessage(&signals[3], adbms_can.mainboard->adbms.undertemperature_fault_, 1, 1, 0);												// undertemp fault
	populateRawMessage(&signals[4], adbms_can.mainboard->adbms.overtemperature_fault_, 1, 1, 0);											 	// overemp fault
	populateRawMessage(&signals[5], adbms_can.mainboard->overcurrent_fault, 1, 1, 0);														 	// overcurrent fault
	populateRawMessage(&signals[6], adbms_can.mainboard->external_fault, 1, 1, 0);													   			// external fault
	populateRawMessage(&signals[7], (adbms_can.mainboard->adbms.openwire_fault_ || adbms_can.mainboard->adbms.openwire_temp_fault_), 1, 1, 0);	// open wire fault
	encodeSignals(data, 8, signals[0], signals[1], signals[2], signals[3], signals[4], signals[5], signals[6], signals[7]);
}

void populateBMS_Status(uint8_t *data)
{
	RawCanSignal signals[7];

	populateRawMessage(&signals[0], 0, 8, 1, 0);		 // BMS State
	populateRawMessage(&signals[1], adbms_can.mainboard->imd_status, 8, 1, 0);		 // IMD State
	populateRawMessage(&signals[2], adbms_can.mainboard->adbms.max_temp, 8, 1, -40); // max cell temp
	populateRawMessage(&signals[3], adbms_can.mainboard->adbms.min_temp, 8, 1, -40); // min cell temp
	populateRawMessage(&signals[4], adbms_can.mainboard->adbms.max_v, 8, 0.012, 2);	 // max cell voltage
	populateRawMessage(&signals[5], adbms_can.mainboard->adbms.min_v, 8, 0.012, 2);	 // min cell voltage
	populateRawMessage(&signals[6], 0, 8, 0.5, 0);									 // BMS SOC
	encodeSignals(data, 7, signals[0], signals[1], signals[2], signals[3], signals[4], signals[5], signals[6]);
}


void populateBMS_VoltageMessages(uint8_t *data, int volt_msg_num)
{
	RawCanSignal signals[8];
	for(int i = 0; i < NUM_DATA_CAN_VOLTAGES_PER_MSG; i++){
		populateRawMessage(&signals[i], adbms_can.mainboard->adbms.voltages[volt_msg_num * NUM_DATA_CAN_VOLTAGES_PER_MSG + i], 8, 0.012, 2);
	}
	populateRawMessage(&signals[7], 0, 8, 0.004, 0);	// OCV msg that is legacy from BQ code and only included for backwards compatibility
	// num_per_msg + 1 because includes the added OCV msg
	encodeSignals(data, NUM_DATA_CAN_VOLTAGES_PER_MSG+1, signals[0], signals[1], signals[2], signals[3], signals[4], signals[5], signals[6], signals[7]);
}

void populateBMS_TemperatureMessages(uint8_t *data, int temp_num)
{
	RawCanSignal signals[8];
	for(int i = 0; i < NUM_DATA_CAN_TEMPS_PER_MSG; i++){
		populateRawMessage(&signals[i], adbms_can.mainboard->adbms.temperatures[temp_num * NUM_DATA_CAN_TEMPS_PER_MSG + i], 8, 1, -40);
	}
	encodeSignals(data, NUM_DATA_CAN_TEMPS_PER_MSG, signals[0], signals[1], signals[2], signals[3], signals[4], signals[5], signals[6], signals[7]);
}
