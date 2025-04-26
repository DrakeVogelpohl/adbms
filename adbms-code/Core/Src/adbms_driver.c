#include "adbms_driver.h"

/**
*******************************************************************************
* Function: Pec15_Calc
* @brief CRC15 Pec Calculation Function
*
* @details This function calculates and return the CRC15 value
*	   
* Parameters:
* @param [in] len      Number of bytes that will be used to calculate a PEC (Data length)
*
* @param [in] *data    Array of data that will be used to calculate a PEC (Data pointer)
*
* @return CRC15_Value
*
*******************************************************************************
*/
uint16_t Pec15_Calc(uint8_t len, uint8_t *data)
{
  uint16_t remainder,addr;
  remainder = 16; /* initialize the PEC */
  for (uint8_t i = 0; i<len; i++) /* loops for each byte in data array */
  {
    addr = (((remainder>>7)^data[i])&0xff);/* calculate PEC table address */
    remainder = ((remainder<<8)^Crc15Table[addr]);
  }
  return(remainder*2);/* The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2 */
}

/**
*******************************************************************************
* Function: Pec10_Calc
* @brief CRC10 Pec Calculation Function
*
* @details This function calculates and return the CRC10 value
*	   
* Parameters:
* @param [in] isRxCmd   Bool to set if the array is from a received buffer 
*
* @param [in] len       Number of bytes that will be used to calculate a PEC (Data length)
*
* @param [in] *data     Array of data that will be used to calculate a PEC (Data pointer)
*
* @return CRC10_Value
*
*******************************************************************************
*/
uint16_t Pec10_Calc(bool isRxCmd, int len, uint8_t *data)
{
    uint16_t nRemainder = 16u; /* PEC_SEED */
    /* x10 + x7 + x3 + x2 + x + 1 <- the CRC10 polynomial 100 1000 1111 */
    uint16_t nPolynomial = 0x8Fu;
    uint8_t nByteIndex, nBitIndex;
  
    for (nByteIndex = 0u; nByteIndex < len; ++nByteIndex)
    {
        /* Bring the next byte into the remainder. */
        nRemainder ^= (uint16_t)((uint16_t)data[nByteIndex] << 2u);
 
        /* Perform modulo-2 division, a bit at a time.*/
        for (nBitIndex = 8u; nBitIndex > 0u; --nBitIndex)
        {
            /* Try to divide the current data bit. */
            if ((nRemainder & 0x200u) > 0u)
            {
                nRemainder = (uint16_t)((nRemainder << 1u));
                nRemainder = (uint16_t)(nRemainder ^ nPolynomial);
            }
            else
            {
                nRemainder = (uint16_t)(nRemainder << 1u);
            }
        }
    }
 
    /* If array is from received buffer add command counter to crc calculation */
    if (isRxCmd == true)
    {  
        nRemainder ^= (uint16_t)(((uint16_t)data[len] & (uint8_t)0xFC) << 2u);
    }
    /* Perform modulo-2 division, a bit at a time */
    for (nBitIndex = 6u; nBitIndex > 0u; --nBitIndex)
    {
        /* Try to divide the current data bit */
        if ((nRemainder & 0x200u) > 0u)
        {
            nRemainder = (uint16_t)((nRemainder << 1u));
            nRemainder = (uint16_t)(nRemainder ^ nPolynomial);
        }
        else
        {
            nRemainder = (uint16_t)((nRemainder << 1u));
        }
    }
    return ((uint16_t)(nRemainder & 0x3FFu));
}

/**
 *******************************************************************************
 * Function: UnderOver_Voltage_Threshold
 * @brief Converts a float into Voltage Threshold 
 *
 * @details This function takes a float and converts it into the 12 bits that 
 *          ADBMS config expects. 
 *
 * Parameters:
 *
 * @param [in]  voltage       Over or Under Voltage Threshold
 *
 * @return VoltageThreshold_value
 *
 *******************************************************************************
*/
uint16_t Set_UnderOver_Voltage_Threshold(float voltage)
{
  uint16_t v_th_value;
  uint8_t rbits = 12;
  voltage = (voltage - 1.5);
  voltage = voltage / (16 * 0.000150);
  v_th_value = (uint16_t )(voltage + 2 * (1 << (rbits - 1)));
  v_th_value &= 0xFFF;
  return v_th_value;
}

void ADBMS_Set_Config_A(cfa_ *cfg_a, uint8_t **cfg_a_tx_buffer)
{
    for(uint8_t cic = 0; cic < NUM_CHIPS; cic++)
    {
        cfg_a_tx_buffer[cic][0] = (uint8_t)(((cfg_a[cic].refon & 0x01) << 7) | (cfg_a[cic].cth & 0x07));
        cfg_a_tx_buffer[cic][1] = (uint8_t)(cfg_a[cic].flag_d & 0xFF);
        cfg_a_tx_buffer[cic][2] = (uint8_t)(((cfg_a[cic].soakon & 0x01) << 7) | ((cfg_a[cic].owrng & 0x01) << 6) | ((cfg_a[cic].owa & 0x07) << 3));
        cfg_a_tx_buffer[cic][3] = (uint8_t)(cfg_a[cic].gpo & 0x00FF);
        cfg_a_tx_buffer[cic][4] = (uint8_t)((cfg_a[cic].gpo & 0x0300) >> 8);
        cfg_a_tx_buffer[cic][5] = (uint8_t)(((cfg_a[cic].snap & 0x01) << 5) | ((cfg_a[cic].mute_st & 0x01) << 4) | ((cfg_a[cic].comm_bk & 0x01) << 3) | (cfg_a[cic].fc & 0x07));
    }
}

void ADBMS_Set_Config_B(cfb_ *cfg_b, uint8_t **cfg_b_tx_buffer)
{
    for(uint8_t cic = 0; cic < NUM_CHIPS; cic++)
    {
        cfg_b_tx_buffer[cic][0] = (uint8_t)(cfg_b[cic].vuv & 0x0FF);
        cfg_b_tx_buffer[cic][1] = (uint8_t)(((cfg_b[cic].vov & 0x00F) << 4) | ((cfg_b[cic].vuv & 0xF00) >> 8));
        cfg_b_tx_buffer[cic][2] = (uint8_t)((cfg_b[cic].vov & 0xFF0) >> 4);
        cfg_b_tx_buffer[cic][3] = (uint8_t)(((cfg_b[cic].dtmen & 0x01) << 7) | ((cfg_b[cic].dtrng & 0x01) << 6) | (cfg_b[cic].dcto & 0x3F));
        cfg_b_tx_buffer[cic][4] = (uint8_t)(cfg_b[cic].dcc & 0x00FF);
        cfg_b_tx_buffer[cic][5] = (uint8_t)((cfg_b[cic].dcc & 0xFF00) >> 8);
    }
}

void ADBMS_WakeUP_ICs(SPI_HandleTypeDef *hspi)
{
    uint8_t dummy_msg[12] = {0};

    for(uint8_t i = 0; i < NUM_CHIPS; i++){
        // Blocking Transmit the msg
        if (HAL_SPI_Transmit(hspi, dummy_msg, 8, SPI_TIME_OUT) != HAL_OK)
        {
            // TODO: do something if fails
        }

        // wait at least t_wake to let the signal propagate
        HAL_Delay(1);
    }
}

void ADBMS_Write_CMD(SPI_HandleTypeDef *hspi, uint16_t tx_cmd)
{
    uint8_t spi_dataBuf[4];
    spi_dataBuf[0] = (uint8_t)(tx_cmd >> 8);
    spi_dataBuf[1] = (uint8_t)(tx_cmd);

    uint16_t cmd_pec = Pec15_Calc(2, spi_dataBuf);
    spi_dataBuf[2] = (uint8_t)(cmd_pec >> 8);
    spi_dataBuf[3] = (uint8_t)(cmd_pec);

    // Blocking Transmit the cmd
    if (HAL_SPI_Transmit(hspi, spi_dataBuf, CMD_LEN + PEC_LEN, SPI_TIME_OUT) != HAL_OK)
    {
        // TODO: do something if fails
    }
}

void ADBMS_Write_Data(SPI_HandleTypeDef *hspi, uint16_t tx_cmd, uint8_t **data, uint8_t *spi_dataBuf)
{
    spi_dataBuf[0] = (uint8_t)(tx_cmd >> 8);
    spi_dataBuf[1] = (uint8_t)(tx_cmd);

    uint16_t cmd_pec = Pec15_Calc(2, spi_dataBuf);
    spi_dataBuf[2] = (uint8_t)(cmd_pec >> 8);
    spi_dataBuf[3] = (uint8_t)(cmd_pec);

    // Decrementing because sends to last chip on the stack first
    for(uint8_t cic = NUM_CHIPS; cic > 0; cic--){
        // Copy over data from data ptr
        for(uint8_t cbyte = 0; cbyte < DATA_LEN; cbyte++){
            spi_dataBuf[4 + cbyte + (cic*(DATA_LEN + PEC_LEN))] = data[cic][cbyte];
        }

        // Caclulate PEC10
        uint16_t data_pec = Pec10_Calc(false, DATA_LEN, data[cic]);  
        spi_dataBuf[4 + DATA_LEN + (cic*(DATA_LEN + PEC_LEN))] = (uint8_t)(data_pec >> 8);
        spi_dataBuf[4 + DATA_LEN + 1 + (cic*(DATA_LEN + PEC_LEN))] = (uint8_t)(data_pec);
    }

    // Blocking Transmit the cmd and data
    if (HAL_SPI_Transmit(hspi, spi_dataBuf, DATABUF_LEN, SPI_TIME_OUT) != HAL_OK)
    {
        // TODO: do something if fails
    }
}

bool ADBMS_Read_Data(SPI_HandleTypeDef *hspi, uint16_t tx_cmd, uint8_t **dataBuf, uint8_t *spi_dataBuf)
{
    uint8_t spi_tx_dataBuf[DATABUF_LEN] = {0};
    spi_tx_dataBuf[0] = (uint8_t)(tx_cmd >> 8);
    spi_tx_dataBuf[1] = (uint8_t)(tx_cmd);

    uint16_t cmd_pec = Pec15_Calc(2, spi_dataBuf);
    spi_tx_dataBuf[2] = (uint8_t)(cmd_pec >> 8);
    spi_tx_dataBuf[3] = (uint8_t)(cmd_pec);

    // Blocking Transmit Receive the cmd and data
    if (HAL_SPI_TransmitReceive(hspi, spi_tx_dataBuf, spi_dataBuf, DATABUF_LEN, SPI_TIME_OUT) != HAL_OK)
    {
        // TODO: do something if fails
    }

    // Discard data received during transmit phase
    uint8_t *rx_dataBuf = spi_dataBuf + CMD_LEN + PEC_LEN;

    // Move the incoming data from the spi data buffer to the correspoding data buffer array in memory
    bool pec_error = 0;
    for(uint8_t cic = 0; cic < NUM_CHIPS; cic++)
    {
        for(uint8_t cbyte = 0; cbyte < DATA_LEN; cbyte++)
        {
            dataBuf[cic][cbyte] = rx_dataBuf[cbyte + (DATA_LEN+PEC_LEN)*cic];
        }
        uint16_t rx_pec = (uint16_t)(((rx_dataBuf[DATA_LEN + (DATA_LEN+PEC_LEN)*cic] & 0x03) << 8) | rx_dataBuf[DATA_LEN + 1 + (DATA_LEN+PEC_LEN)*cic]);
        uint16_t calc_pec = (uint16_t)Pec10_Calc(true, DATA_LEN, dataBuf[cic]);
        pec_error |= (rx_pec != calc_pec);
    }

    return pec_error;
}