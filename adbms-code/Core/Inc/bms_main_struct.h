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
	bool ECU_Cmd_Close_Contactors;
	float Inverter_DC_Voltage;

	// chrager
	bool charger_pin;
	bool charger_enable;
	bool charger_status;
	float charger_voltage;
	float charger_current;

	// timeouts
	bool timeout_fault;
	float ecu_last_msg_time;
	bool ecu_timeout_fault_;
	float inverter_last_msg_time;
	bool inverter_timeout_fault;
	float charger_last_msg_time;
	bool charger_timeout_fault;

	bool comms_6822_state;
	uint32_t start_time;
} mainboard_;

#endif // ADBMS_MAIN_STRUCT_H
