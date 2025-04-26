#include "adbms_driver.h"

/* Precomputed CRC15 Table */
const uint16_t Crc15Table[256] =
{
  0x0000,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,
  0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
  0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
  0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
  0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
  0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
  0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
  0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
  0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
  0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
  0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
  0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
  0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
  0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
  0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
  0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
  0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
  0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
  0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
  0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
  0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
  0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
  0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
};

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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);
    for(uint8_t i = 0; i < NUM_CHIPS; i++){
        // Blocking Transmit the msg
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    	HAL_Delay(1);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
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
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(hspi, spi_dataBuf, CMD_LEN + PEC_LEN, SPI_TIME_OUT) != HAL_OK)
    {
        // TODO: do something if fails
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
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
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(hspi, spi_dataBuf, DATABUF_LEN, SPI_TIME_OUT) != HAL_OK)
    {
        // TODO: do something if fails
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
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
    uint8_t dBuf[DATABUF_LEN] = {0};
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, spi_tx_dataBuf, 4, SPI_TIME_OUT);
    HAL_SPI_Receive(hspi, dBuf, DATABUF_LEN-4, SPI_TIME_OUT);
//    if (HAL_SPI_TransmitReceive(hspi, spi_tx_dataBuf, spi_dataBuf, DATABUF_LEN, SPI_TIME_OUT) != HAL_OK)
//    {
//        // TODO: do something if fails
//    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

    for(uint8_t i = 0; i < DATABUF_LEN-4; i++)
    {
    	printf("byte%d: 0x%02x\n", i, dBuf[i]);
    }

    // Discard data received during transmit phase
//    uint8_t *rx_dataBuf = spi_dataBuf + CMD_LEN + PEC_LEN;
    uint8_t *rx_dataBuf = dBuf;

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
