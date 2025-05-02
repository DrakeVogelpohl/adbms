#ifndef ADBMS_MAIN_STRUCT_H
#define ADBMS_MAIN_STRUCT_H

#include "adbms_interface.h"
#include "current_driver.h"
#include "virtual_timer.h"
#include "stm32f4xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "main.h"


typedef struct
{
	// AD Chips
	adbms_ adbms;

	// Timer Group
	timer_group_ *tg;
	timer_group_ *tg2;

	// handles
	ADC_HandleTypeDef *hadc;
	CAN_HandleTypeDef *hcan_drive;
	CAN_HandleTypeDef *hcan_data;

	// faults
	bool external_fault;
	bool bms_fault;

	// current
	float current;
	float current_offset;
	bool overcurrent_fault;

	// external values
	bool shutdown_present;
	bool imd_status;

	// chrager
	bool charger_pin;
	bool charger_enable;

	bool comms_6822_state;
	uint32_t start_time;
} mainboard_;

#endif // ADBMS_MAIN_STRUCT_H
