#include "adbms_interface.h"

void ADBMS_Interface_Initialize(adbms_ *adbms, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csb_pinBank, uint16_t csb_pin)
{
    ADBMS_Init(&adbms->ICs, hspi, csb_pinBank, csb_pin);

    // Define which VoltageReg groups to read
    adbms->vregs[0] = RDCVA;
    adbms->vregs[1] = RDCVB;
    adbms->vregs[2] = RDCVC;
    adbms->vregs[3] = RDCVD;
    adbms->vregs[4] = RDCVE;

    // Define which TempReg groups to read
    adbms->tregs[0] = RDAUXA;
    adbms->tregs[1] = RDAUXB;
    adbms->tregs[2] = RDAUXC;
    adbms->tregs[3] = RDAUXD;

    // Set initial configurations
    for (uint8_t cic = 0; cic < NUM_CHIPS; cic++)
    {
        // Init config A
        adbms->cfa[cic].refon = 1;
        adbms->cfa[cic].gpo = 0x3FF;  // all gpo tunred on

        // Init config B
        adbms->cfb[cic].vuv = Set_UnderOver_Voltage_Threshold(UNDERVOLTAGE);
        adbms->cfb[cic].vov = Set_UnderOver_Voltage_Threshold(OVERVOLTAGE);
    }
    // Init sensing cmd
    adbms->adcv.cont = 1;

    // Package config and sensing structs into transmitable data
    ADBMS_Set_Config_A(adbms->cfa, adbms->ICs.cfg_a);
    ADBMS_Set_Config_B(adbms->cfb, adbms->ICs.cfg_b);
    ADBMS_Set_ADCV(adbms->adcv, &adbms->ICs.adcv);
    ADBMS_Set_ADAX(adbms->adax, &adbms->ICs.adax);

    // Write Config 
    ADBMS_WakeUP_ICs_Polling();
    ADBMS_WakeUP_ICs_Polling();
    ADBMS_Write_Data_RegGrp_Polling(adbms->ICs.hspi, WRCFGA, adbms->ICs.cfg_a, adbms->ICs.spi_tx_dataBuf);
    ADBMS_WakeUP_ICs_Polling();
    ADBMS_Write_Data_RegGrp_Polling(adbms->ICs.hspi, WRCFGB, adbms->ICs.cfg_b, adbms->ICs.spi_tx_dataBuf);

    // Turn on sensing
    ADBMS_Write_CMD_Polling(adbms->ICs.hspi, adbms->ICs.adcv);
    HAL_Delay(1);
    ADBMS_Write_CMD_Polling(adbms->ICs.hspi, adbms->ICs.adax);
    HAL_Delay(8); // ADCs are updated at their conversion rate of 1ms
}

void UpdateADInternalFault(adbms_ *adbms)
{
    // check overvoltage fault
    adbms->overvoltage_fault_ = adbms->overvoltage_fault_ || (adbms->max_v > OVERVOLTAGE);

    // check undervoltage fault
    adbms->undervoltage_fault_ = adbms->undervoltage_fault_ || (adbms->min_v < UNDERVOLTAGE);

    // check overtemperature fault
    adbms->overtemperature_fault_ = adbms->overtemperature_fault_ || (adbms->max_temp > OVERTEMP);

    // check undertemperature fault
    adbms->undertemperature_fault_ = adbms->undertemperature_fault_ || (adbms->min_temp < UNDERTEMP);

    // TODO: check status regs for faults - need calcuate status reg values fn that handles status reg pec fualts
}


void ADBMS_UpdateVoltages(adbms_ *adbms)
{
    // get voltages from ADBMS
    uint8_t pec = 0;
    ADBMS_WakeUP_ICs_Polling();

    pec += ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDCVA, (adbms->ICs.cell + 0 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_tx_dataBuf);
    pec += ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDCVB, (adbms->ICs.cell + 1 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_tx_dataBuf);
    pec += ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDCVC, (adbms->ICs.cell + 2 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_tx_dataBuf);
    pec += ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDCVD, (adbms->ICs.cell + 3 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_tx_dataBuf);
    pec += ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDCVE, (adbms->ICs.cell + 4 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_tx_dataBuf);
    adbms->total_pec_failures += pec;
    adbms->voltage_pec_failure = pec;

    // calulate new values with the updated raw ones
     ADBMS_CalculateValues_Voltages(adbms);
}

void ADBMS_UpdateTemps(adbms_ *adbms)
{
    // get temps from ADBMS
    uint8_t pec = 0;
    ADBMS_WakeUP_ICs_Polling();
    pec += ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDAUXA, (adbms->ICs.aux + 0 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec += ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDAUXB, (adbms->ICs.aux + 1 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec += ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDAUXC, (adbms->ICs.aux + 2 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec += ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDAUXD, (adbms->ICs.aux + 3 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    adbms->total_pec_failures += pec;
    adbms->temp_pec_failure = pec;

    // need to start new poll for conversion before next read (no continous mode)
    ADBMS_Write_CMD_Polling(adbms->ICs.hspi, adbms->ICs.adax);

    // calulate new values with the updated raw ones
    ADBMS_CalculateValues_Temps(adbms);
}

void ADBMS_CalculateValues_Voltages(adbms_ *adbms)
{
    // reset current pec failures if there is no current failure
    if(!adbms->voltage_pec_failure && !adbms->temp_pec_failure && !adbms->status_reg_pec_failure) { 
        adbms->current_pec_failures = 0;
    }

    // if there is a pec failure, process it and don't update values
    if(adbms->voltage_pec_failure) {
        adbms->current_pec_failures += adbms->voltage_pec_failure;
        if(adbms->current_pec_failures > PEC_FAILURE_THRESHOLD) {
            adbms->pec_fault_ = 1;
        }else {
            adbms->pec_fault_ = 0;
        }
        return;
    }

    // calculate the total, max, and min voltage
    adbms->total_v = 0;
    adbms->max_v = 0;
    adbms->min_v = FLT_MAX;
    for (uint8_t cic = 0; cic < NUM_CHIPS; cic++)
    {
        for (uint8_t creg_grp = 0; creg_grp < NUM_VREG_GRPS_READ; creg_grp++)
        {
            for (uint8_t cbyte = 0; cbyte < DATA_LEN; cbyte+=2)
            {
                if(creg_grp*DATA_LEN/2 + cbyte/2 >= NUM_VOLTAGES_CHIP) break;   // only read 14 when getting 15 -- TODO CHANGE COMMENT
                int16_t raw_val = (((uint16_t)adbms->ICs.cell[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte + 1]) << 8) | adbms->ICs.cell[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte];
                float curr_voltage = ADBMS_getVoltage(raw_val);
                adbms->voltages[cic*NUM_VOLTAGES_CHIP + creg_grp*DATA_LEN/2 + cbyte/2] = curr_voltage;

                adbms->total_v += curr_voltage;
                if (curr_voltage > adbms->max_v){
                    adbms->max_v = curr_voltage;
                }
                if (curr_voltage < adbms->min_v){
                    adbms->min_v = curr_voltage;
                }
            }
        }
    }

    // calculate the avg voltage
    adbms->avg_v = adbms->total_v / (NUM_CHIPS * NUM_VOLTAGES_CHIP);
}

void ADBMS_CalculateValues_Temps(adbms_ *adbms)
{
    // reset current pec failures if there is no current failure
    if(!adbms->voltage_pec_failure && !adbms->temp_pec_failure && !adbms->status_reg_pec_failure) { 
        adbms->current_pec_failures = 0;
    }

    // if there is a pec failure, process it and don't update values
    if(adbms->temp_pec_failure) {
        adbms->current_pec_failures += adbms->temp_pec_failure;
        if(adbms->current_pec_failures > PEC_FAILURE_THRESHOLD) {
            adbms->pec_fault_ = 1;
        }else {
            adbms->pec_fault_ = 0;
        }
        return;
    }
    
    // calculate the total, max, and min temp
    float total_temp = 0.0;
    adbms->max_temp = 0;
    adbms->min_temp = FLT_MAX;
    bool openwire_temp_fault = false;
    for (int cic = 0; cic < NUM_CHIPS; cic++)
    {
        for (uint8_t creg_grp = 0; creg_grp < AUX_REG_GRP; creg_grp++)
        {
            for (uint8_t cbyte = 0; cbyte < DATA_LEN; cbyte+=2)
            {
                // skip because only want temps 2-10
                if((creg_grp==0 && cbyte <= 2) || creg_grp*DATA_LEN/2 + cbyte/2 >= AUX_GPIO) continue;

                int16_t raw_val = (((uint16_t)adbms->ICs.aux[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte + 1]) << 8) | adbms->ICs.aux[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte];
                float raw_temp_voltage = ADBMS_getVoltage(raw_val);

                // get ref voltage from status reg - not getting status regs because takes too long
                //float vref = ADBMS_getVoltage(ICs[i].stata.vref2);
                float vref = 3; // 3V defined in the datasheet
                if (vref - raw_temp_voltage < 1e-1)
                    openwire_temp_fault = true;

                float curr_temp = getTemp(raw_temp_voltage, vref);
                adbms->temperatures[cic*NUM_TEMPS_CHIP + creg_grp*DATA_LEN/2 + cbyte/2 - 2] = curr_temp;  // -2 because offset for skipped temps
                total_temp += curr_temp;
                if (curr_temp > adbms->max_temp)
                    adbms->max_temp = curr_temp;
                if (curr_temp < adbms->min_temp)
                    adbms->min_temp = curr_temp;
            }
        }
    }
    adbms->openwire_temp_fault_ = adbms->openwire_temp_fault_ || openwire_temp_fault;
    // calculate the avg temp
    adbms->avg_temp = total_temp / (NUM_CHIPS * NUM_TEMPS_CHIP);
    
}

void ADBMS_DMA_Complete(adbms_ *adbms)
{
    // Start new Transmit Receive DMA
    ADBMS_TransmitReceive_Reg_DMA(&adbms->ICs);

    // Fill new Tx buf for next DMA
    adbms->vreg_index += 1;
    if(adbms->vreg_index >= NUM_VREG_GRPS_READ) adbms->vreg_index = 0;

    ADBMS_Pack_CMD(adbms->vregs[adbms->vreg_index], adbms->ICs.spi_tx_dataBuf);

    // Process Rx data
    bool pec = ADBMS_Process_Read_Data_RegGrp(adbms->ICs.spi_rx_dataBuf, adbms->ICs.cell);
    adbms->total_pec_failures += pec;
    adbms->voltage_pec_failure = pec;
    if(!pec) ADBMS_CalculateValue_Grp_Voltages(adbms);
}

void ADBMS_CalculateValue_Grp_Voltages(adbms_ *adbms)
{
    // reset current pec failures if there is no current failure
    if(!adbms->voltage_pec_failure && !adbms->temp_pec_failure && !adbms->status_reg_pec_failure) { 
        adbms->current_pec_failures = 0;
    }

    // if there is a pec failure, process it and don't update values
    if(adbms->voltage_pec_failure) {
        adbms->current_pec_failures += adbms->voltage_pec_failure;
        if(adbms->current_pec_failures > PEC_FAILURE_THRESHOLD) {
            adbms->pec_fault_ = 1;
        }else {
            adbms->pec_fault_ = 0;
        }
        return;
    }
    
    for (uint8_t cic = 0; cic < NUM_CHIPS; cic++)
    {
        uint8_t creg_grp = adbms->vreg_index - 1;   // -1 because already increameted for next DMA
        if(creg_grp < 0) creg_grp = NUM_VREG_GRPS_READ;

        for (uint8_t cbyte = 0; cbyte < DATA_LEN; cbyte+=2)
        {
            if(creg_grp*DATA_LEN/2 + cbyte/2 >= NUM_VOLTAGES_CHIP) break;   // only read 14 when getting 15 -- TODO CHANGE COMMENT
            int16_t raw_val = (((uint16_t)adbms->ICs.cell[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte + 1]) << 8) | adbms->ICs.cell[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte];
            float curr_voltage = ADBMS_getVoltage(raw_val);
            adbms->voltages[cic*NUM_VOLTAGES_CHIP + creg_grp*DATA_LEN/2 + cbyte/2] = curr_voltage;
        }
    }
}

void cellBalanceOn(adbms_ *adbms)
{
    // Turn on CB indication LED
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

    for (int cic = 0; cic < NUM_CHIPS; cic++)
    {
        uint16_t dcc = 0;
        for (int cvoltage = 0; cvoltage < NUM_VOLTAGES_CHIP; cvoltage++)
        {
            float curr_v = adbms->voltages[cic * NUM_VOLTAGES_CHIP + cvoltage];
            if ((curr_v - adbms->min_v) > CB_THRESHOLD && curr_v > CB_MIN_V_THRESHOLD)
            {
                dcc |= 1 << cvoltage;
            }
        }
        adbms->cfb[cic].dcc = dcc;
        // adbms->cfb[cic].dcc = 0xF;   // for testing
    }
    ADBMS_Set_Config_B(adbms->cfb, adbms->ICs.cfg_b);
    ADBMS_Write_Data_RegGrp_Polling(adbms->ICs.hspi, WRCFGB, adbms->ICs.cfg_b, adbms->ICs.spi_tx_dataBuf);
}

void cellBalanceOff(adbms_ *adbms)
{
    // Turn off CB indication LED
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

    for (int cic = 0; cic < NUM_CHIPS; cic++)
    {
        adbms->cfb[cic].dcc = 0;
    }
    ADBMS_Set_Config_B(adbms->cfb, adbms->ICs.cfg_b);
    ADBMS_Write_Data_RegGrp_Polling(adbms->ICs.hspi, WRCFGB, adbms->ICs.cfg_b, adbms->ICs.spi_tx_dataBuf);
}

void UpdateOWCFault(adbms_ *adbms)
{
    // check openwire fault
    ADBMS_WakeUP_ICs_Polling();
    cellBalanceOff(adbms);   // need to turn off cell balancing to check for OWC

    /// OWC EVEN Check
    adbms->adsv.cont = 1;
    adbms->adsv.ow = 1; // Enable OW on even-channel 
    ADBMS_Set_ADSV(adbms->adsv, &adbms->ICs.adsv);
    ADBMS_Write_CMD_Polling(adbms->ICs.hspi, adbms->ICs.adsv);
    HAL_Delay(8);    // S-Channels are updated at 8ms

    // Get new s-channel voltages
    bool pec = 0;
    ADBMS_WakeUP_ICs_Polling();
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVA, (adbms->ICs.scell + 0 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVB, (adbms->ICs.scell + 1 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVC, (adbms->ICs.scell + 2 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVD, (adbms->ICs.scell + 3 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVE, (adbms->ICs.scell + 4 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);

    if(pec){
        adbms->current_owc_failures += 1;
        if(adbms->current_owc_failures > PEC_FAILURE_THRESHOLD){
            adbms->pec_fault_ = 1;
        }
        return;
    }else adbms->current_owc_failures = 0;

    for (uint8_t cic = 0; cic < NUM_CHIPS; cic++)
    {
        uint8_t num_reg_grps = NUM_VOLTAGES_CHIP / VOLTAGES_REG_GRP + (NUM_VOLTAGES_CHIP % VOLTAGES_REG_GRP != 0);
        for (uint8_t creg_grp = 0; creg_grp < num_reg_grps; creg_grp++)
        {
            for (uint8_t cbyte = 0; cbyte < DATA_LEN; cbyte+=2)
            {
                if(creg_grp*DATA_LEN/2 + cbyte/2 >= NUM_VOLTAGES_CHIP) break;   // only read 14 when getting 15 -- TODO CHANGE COMMENT
                int16_t raw_val = (((uint16_t)adbms->ICs.scell[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte + 1]) << 8) | adbms->ICs.scell[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte];
                if (ADBMS_getVoltage(raw_val) < 0.5)
                {
                    adbms->openwire_fault_ = 1;
                    return;
                }
            }
        }
    }

    /// OWC ODD Check
    adbms->adsv.cont = 1;
    adbms->adsv.ow = 2; // Enable OW on odd-channel 
    ADBMS_Set_ADSV(adbms->adsv, &adbms->ICs.adsv);
    ADBMS_Write_CMD_Polling(adbms->ICs.hspi, adbms->ICs.adsv);
    HAL_Delay(8);    // S-Channels are updated at 8ms

    // Get new s-channel voltages
    ADBMS_WakeUP_ICs_Polling();
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVA, (adbms->ICs.scell + 0 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVB, (adbms->ICs.scell + 1 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVC, (adbms->ICs.scell + 2 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVD, (adbms->ICs.scell + 3 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);
    pec |= ADBMS_Read_Data_RegGrp_Polling(adbms->ICs.hspi, RDSVE, (adbms->ICs.scell + 4 * NUM_CHIPS * DATA_LEN), adbms->ICs.spi_rx_dataBuf);

    if(pec){
        adbms->current_owc_failures += 1;
        if(adbms->current_owc_failures > PEC_FAILURE_THRESHOLD){
            adbms->pec_fault_ = 1;
        }
        return;
    }else adbms->current_owc_failures = 0;

    for (uint8_t cic = 0; cic < NUM_CHIPS; cic++)
    {
        uint8_t num_reg_grps = NUM_VOLTAGES_CHIP / VOLTAGES_REG_GRP + (NUM_VOLTAGES_CHIP % VOLTAGES_REG_GRP != 0);
        for (uint8_t creg_grp = 0; creg_grp < num_reg_grps; creg_grp++)
        {
            for (uint8_t cbyte = 0; cbyte < DATA_LEN; cbyte+=2)
            {
                if(creg_grp*DATA_LEN/2 + cbyte/2 >= NUM_VOLTAGES_CHIP) break;   // only read 14 when getting 15 -- TODO CHANGE COMMENT
                int16_t raw_val = (((uint16_t)adbms->ICs.scell[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte + 1]) << 8) | adbms->ICs.scell[creg_grp * NUM_CHIPS * DATA_LEN + cic * DATA_LEN + cbyte];
                if (ADBMS_getVoltage(raw_val) < 0.5)
                {
                    adbms->openwire_fault_ = 1;
                    return;
                }
            }
        }
    }

    /// Turn off owc
    adbms->adsv.cont = 0;
    adbms->adsv.ow = 0; // Enable OW on odd-channel 
    ADBMS_Set_ADSV(adbms->adsv, &adbms->ICs.adsv);
    ADBMS_Write_CMD_Polling(adbms->ICs.hspi, adbms->ICs.adsv);
    HAL_Delay(1);    // S-Channels are updated at 8ms
}

void ADBMS_Print_Vals(adbms_ *adbms)
{
    // print the total, max, min, and avg voltage
    printf("\nVOLTAGES\n");
    printf("total v: %f\n", adbms->total_v);
    printf("max v: %f\t", adbms->max_v);
    printf("min v: %f\t", adbms->min_v);
    printf("avg v: %f\t", adbms->avg_v);
    printf("max-min: %f\n", adbms->max_v - adbms->min_v);

    // print every voltage
    for (int i = 0; i < NUM_CHIPS; i++)
    {
        for (int j = 0; j < NUM_VOLTAGES_CHIP; j++)
        {
            printf("C%d=%fV\t", (i * NUM_VOLTAGES_CHIP + j + 1), adbms->voltages[i * NUM_VOLTAGES_CHIP + j]);
        }
    }
    printf("\n");

    // print the total, max, min, and avg temp
    printf("\nTEMPS\n");
    printf("max temp: %f\t", adbms->max_temp);
    printf("min temp: %f\t", adbms->min_temp);
    printf("avg temp: %f\n", adbms->avg_temp);

    for (int i = 0; i < NUM_CHIPS; i++)
    {
        for (int j = 0; j < NUM_TEMPS_CHIP; j++)
        {
            printf("T%d=%f\t", (i * NUM_TEMPS_CHIP + j + 1), adbms->temperatures[i * NUM_TEMPS_CHIP + j]);
        }
    }
    printf("\n");

    printf("Faults\n");
    printf("undervoltage: %d\t", adbms->undervoltage_fault_);
    printf("overvoltage: %d\t", adbms->overvoltage_fault_);
    printf("pec: %d\t", adbms->pec_fault_);
    printf("overtemperature: %d\t", adbms->overtemperature_fault_);
    printf("openwire: %d\t", adbms->openwire_fault_);
    printf("openwire_temp: %d\n", adbms->openwire_temp_fault_);
}

void ADBMS_USB_Serial_Print_Vals(adbms_ *adbms)
{
    #define BUFFER_SIZE 3500  // Increase this if more snprintfs are added
    char logBuf[BUFFER_SIZE];
    int len = 0;
    int remaining = BUFFER_SIZE;

    
    len += snprintf(logBuf + len, remaining, "total_v: %f\t", adbms->total_v);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "max_v: %f\t", adbms->max_v);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "min_v: %f\t", adbms->min_v);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "avg_v: %f\t", adbms->avg_v);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "max-min: %f\r\n", adbms->max_v - adbms->min_v);
    remaining = BUFFER_SIZE - len;

    for (int i = 0; i < NUM_CHIPS; i++)
    {
        for (int j = 0; j < NUM_VOLTAGES_CHIP; j++)
        {
            len += snprintf(logBuf + len, remaining, "C%d=%fV\t", 
                            (i * NUM_VOLTAGES_CHIP + j + 1), 
                            adbms->voltages[i * NUM_VOLTAGES_CHIP + j]);
            remaining = BUFFER_SIZE - len;
            if (remaining <= 0) break;
        }
    }
    len += snprintf(logBuf + len, remaining, "\r\n");
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "max_temp: %f\t", adbms->max_temp);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "min_temp: %f\t", adbms->min_temp);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "avg_temp: %f\r\n", adbms->avg_temp);
    remaining = BUFFER_SIZE - len;

    /*for (int i = 0; i < NUM_CHIPS; i++)
    {
        for (int j = 0; j < NUM_TEMPS_CHIP; j++)
        {
            len += snprintf(logBuf + len, remaining, "T%d=%f\t", 
                            (i * NUM_TEMPS_CHIP + j + 1),
                            adbms->temperatures[i * NUM_TEMPS_CHIP + j]);
            remaining = BUFFER_SIZE - len;
            if (remaining <= 0) break;
        }
    }
    len += snprintf(logBuf + len, remaining, "\r\n");
    remaining = BUFFER_SIZE - len;*/

    len += snprintf(logBuf + len, remaining, "undervoltage: %d\t", adbms->undervoltage_fault_);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "overvoltage: %d\t", adbms->overvoltage_fault_);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "pec: %d\t", adbms->pec_fault_);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "overtemperature: %d\t", adbms->overtemperature_fault_);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "openwire: %d\t", adbms->openwire_fault_);
    remaining = BUFFER_SIZE - len;

    len += snprintf(logBuf + len, remaining, "openwire_temp: %d\r\n", adbms->openwire_temp_fault_);
    remaining = BUFFER_SIZE - len;

//    for (int i = 0; i < NUM_CHIPS; i++)
//    {
//    	len += snprintf(logBuf + len, remaining, "dcc%d: %02x\t", i+1, ICs[i].tx_cfgb.dcc);
//    	remaining = BUFFER_SIZE - len;
//    }
//    len += snprintf(logBuf + len, remaining, "\r\n");
//	remaining = BUFFER_SIZE - len;

    if (remaining <= 0) HardFault_Handler();

    CDC_Transmit_FS((uint8_t*) logBuf, strlen(logBuf));
}
