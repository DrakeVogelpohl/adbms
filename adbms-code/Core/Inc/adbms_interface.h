#pragma once

#include "adbms_driver.h"
#include "thermistor_driver.h"
#include "bms_system_prams.h"
#include <float.h>

// Container that holds all the adbms values. 
// Different from the ICs struct that the raw SPI data gets put into.
typedef struct
{
    adbms6830_ICs ICs;

    cfa_ cfa[NUM_CHIPS];
    cfb_ cfb[NUM_CHIPS];
    adcv_ adcv;
    adsv_ adsv;
    adax_ adax;

    float voltages[NUM_CHIPS * NUM_VOLTAGES_CHIP];
    float temperatures[NUM_CHIPS * NUM_TEMPS_CHIP];

    float total_v;
    float max_v;
    float min_v;
    float avg_v;

    float max_temp;
    float min_temp;
    float avg_temp;

    bool undervoltage_fault_;
    bool overvoltage_fault_;
    bool undertemperature_fault_;
    bool overtemperature_fault_;
    bool openwire_fault_;
    bool openwire_temp_fault_;
    bool pec_fault_;

    bool voltage_pec_failure;
    bool temp_pec_failure;
    bool status_reg_pec_failure;
    float current_owc_failures;
    float current_pec_failures;
    float total_pec_failures;
} adbms_;


void ADBMS_Interface_Initialize(adbms_ *adbms, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csb_pinBank, uint16_t csb_pin);

void UpdateADInternalFault(adbms_ *adbms);

void ADBMS_UpdateVoltages(adbms_ *adbms);
void ADBMS_UpdateTemps(adbms_ *adbms);
void UpdateOWCFault(adbms_ *adbms);

void ADBMS_CalculateValues_Voltages(adbms_ *adbms);
void ADBMS_CalculateValues_Temps(adbms_ *adbms);

bool ADBMS_PEC_Check(adbms_ *adbms);

void cellBalanceOn(adbms_ *adbms);
void cellBalanceOff(adbms_ *adbms);

void ADBMS_Print_Vals(adbms_ *adbms);
void ADBMS_USB_Serial_Print_Vals(adbms_ *adbms);
