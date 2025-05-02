#ifndef ADBMS_CAN_H
#define ADBMS_CAN_H

#include "nfr_can_driver.h"
#include "bms_main_struct.h"
#include "stm32f4xx_hal.h"

typedef struct
{
	mainboard_ *mainboard;

	uint32_t TxMailBox_;

	CAN_RxHeaderTypeDef RxHeader_;
	uint8_t rxData_[8];

	// Drive CAN Messages
	CAN_TxHeaderTypeDef TxHeaderSOC_;
	uint8_t txDataSOC_[8];

	CAN_TxHeaderTypeDef TxHeaderFaults_;
	uint8_t txDataFaults_[8];

	CAN_TxHeaderTypeDef TxHeaderStatus_;
	uint8_t txDataStatus_[8];

	CAN_TxHeaderTypeDef TxHeaderCharger_;
	uint8_t txDataCharger_[8];

	// data can messages
	CAN_TxHeaderTypeDef TxHeaderVoltages_;
	uint8_t txDataVoltages_[8];

	CAN_TxHeaderTypeDef TxHeaderTemperatures_;
	uint8_t txDataTemperatures_[8];
} adbms_can_;

void ADBMS_Initialize_Can(mainboard_ *mainboard);

uint8_t send_can_messages(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *TxHeader, uint8_t *data, uint32_t *TxMailBox);

// void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

// CAN Loops
void drive_can_loop();
void data_can_loop();

#endif // ADBMS_CAN_H
