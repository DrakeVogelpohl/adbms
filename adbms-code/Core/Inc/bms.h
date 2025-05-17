#ifndef ADBMS_MAINBOARD_H
#define ADBMS_MAINBOARD_H

#include "bms_main_struct.h"
#include "bms_can.h" 

void bms_mainbaord_setup(SPI_HandleTypeDef *hspi, GPIO_TypeDef *csb_pinBank, uint16_t csb_pin, ADC_HandleTypeDef *hadc, CAN_HandleTypeDef *hcan1, CAN_HandleTypeDef *hcan2);
void bms_mainboard_loop();

void tick_mainboard_timers();

void UpdateValues();
void CheckFaults();

void adbms_owc_loop();

void DMA_Callback();

void send_data_over_printf();
void send_data_over_USB();

#endif // ADBMS_MAINBOARD_H
