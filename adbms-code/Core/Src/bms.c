#include "bms.h"

mainboard_ mainboard;

void bms_mainbaord_setup(SPI_HandleTypeDef *hspi, GPIO_TypeDef *csb_pinBank, uint16_t csb_pin, ADC_HandleTypeDef *hadc, CAN_HandleTypeDef *hcan1, CAN_HandleTypeDef *hcan2)
{
	// initialize handles
	mainboard.hadc = hadc;
	mainboard.hcan_drive = hcan1;
	mainboard.hcan_data = hcan2;

	// get offset for current
	mainboard.current_offset = getCurrentOffset(mainboard.hadc);

	// initialize ad chip;
	ADBMS_Interface_Initialize(&mainboard.adbms, hspi, csb_pinBank, csb_pin);

	// initialize CAN;
	BMS_Initialize_Can(&mainboard);

	// initialize the timers: adbms_mainboard_loop, drive_can, data_can
	timer_ t_adbms = CreateTimer(500, bms_mainboard_loop);
	timer_ t_adbms_owc_check = CreateTimer(30000, adbms_owc_loop);
	timer_ t_drive_can = CreateTimer(100, drive_can_loop);
	timer_ t_data_can = CreateTimer(1000, data_can_loop);
	timer_ timers[NUM_TIMERS] = {t_adbms, t_adbms_owc_check, t_drive_can, t_data_can};
	mainboard.tg = CreateTimerGroup(timers);

	mainboard.start_time = HAL_GetTick();
}

int counter;
void DMA_Callback()
{
	counter += 1;

//	for(uint8_t c = 0; c < DATABUF_LEN; c++){
//		printf("Byte%d: 0x%02x\n", c, mainboard.adbms.ICs.spi_rx_dataBuf[c]);
//	}
	ADBMS_DMA_Complete(&mainboard.adbms);
}

void tick_mainboard_timers()
{
	TickTimerGroup(mainboard.tg);
}

// ADBMS loop that gets ticked
void bms_mainboard_loop()
{
	UpdateValues();
	CheckFaults();
}

// Seprate loop that gets ticked to run OWC
void adbms_owc_loop(){ UpdateOWCFault(&mainboard.adbms); }

void UpdateValues()
{
	printf("callback: %d\n", counter);
    ADBMS_TransmitReceive_Reg_DMA(&mainboard.adbms.ICs);

	// ADBMS values
	// ADBMS_UpdateVoltages(&mainboard.adbms);
	// ADBMS_UpdateTemps(&mainboard.adbms);
	ADBMS_Update_Voltages(&mainboard.adbms);

	UpdateADInternalFault(&mainboard.adbms);

	// update STM32 Pin values
	// reads: shutdown_contactors, IMD_Status, 6822_State
	mainboard.shutdown_present = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1); 	   // shutdown status
	mainboard.imd_status = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8);			   // IMD_Status
	mainboard.comms_6822_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);	   // 6822_State
	mainboard.charger_pin = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);		   // Charger_Pin

	// get current
	mainboard.current = getCurrent(mainboard.hadc) - mainboard.current_offset;
	mainboard.overcurrent_fault = mainboard.current > OVERCURRENT;

	if(ENABLE_PRINTF_DEBUG_COMMS) send_data_over_printf(); 
	if(ENABLE_USB_COMMS) send_data_over_USB(); 
}

void CheckFaults()
{
	// raise fault flag if any fault is true
	// faults are latching
	mainboard.bms_fault = mainboard.bms_fault 
							|| mainboard.adbms.overvoltage_fault_
							|| mainboard.adbms.undervoltage_fault_
							|| mainboard.adbms.overtemperature_fault_
							|| mainboard.adbms.undertemperature_fault_
							|| mainboard.adbms.openwire_fault_
							|| mainboard.adbms.openwire_temp_fault_
							|| mainboard.adbms.pec_fault_
							|| mainboard.overcurrent_fault;

	// write BMS_Status - healthy is high
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, !mainboard.bms_fault);


	// set external faults
	mainboard.external_fault = !mainboard.shutdown_present;

	// Turns on external LED if external fault
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, mainboard.external_fault);

}

void send_data_over_printf()
{
	// AD Prints
	ADBMS_Print_Vals(&mainboard.adbms);

	// Mainboard Prints
	printf("Time: %d\n", (int)(HAL_GetTick() - mainboard.start_time));
//	printf("BMS fault: %d\n", mainboard.bms_fault);
//	printf("External fault: %d\n", mainboard.external_fault);
//	printf("Current: %f\n", mainboard.current);
	
	// TODO Add more prints as needed

}

void send_data_over_USB()
{
	// USB Serial Print ADBMS values 
	ADBMS_USB_Serial_Print_Vals(&mainboard.adbms);

	#define BUFFER_SIZE 256  // Increase this if more snprintfs are added
    char logBuf[BUFFER_SIZE];
    int len = 0;
    int remaining = BUFFER_SIZE;

    len += snprintf(logBuf + len, remaining, "Time: %d\r\n", (int)(HAL_GetTick() - mainboard.start_time));
    remaining = BUFFER_SIZE - len;

//	len += snprintf(logBuf + len, remaining, "Current: %f\r\n", mainboard.current);
//    remaining = BUFFER_SIZE - len;
//
//	len += snprintf(logBuf + len, remaining, "BMS_fault: %d\r\n", mainboard.bms_fault);
//    remaining = BUFFER_SIZE - len;
//
//	len += snprintf(logBuf + len, remaining, "External_fault: %d\r\n", mainboard.external_fault);
//    remaining = BUFFER_SIZE - len;

	if (remaining <= 0) HardFault_Handler();

	CDC_Transmit_FS((uint8_t*) logBuf, strlen(logBuf));
}
