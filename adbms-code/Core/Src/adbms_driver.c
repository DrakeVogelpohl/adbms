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

void ADBMS_WakeUP_ICs(SPI_HandleTypeDef *hspi){
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

void ADBMS_Write_Data(SPI_HandleTypeDef *hspi, uint8_t *spi_dataBuf, uint16_t tx_cmd, uint8_t **data)
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

bool ADBMS_Read_Data(SPI_HandleTypeDef *hspi, uint8_t *spi_dataBuf, uint16_t tx_cmd, uint16_t **dataBuf)
{
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

    // Blocking Receive the data
    uint8_t *rx_dataBuf = spi_dataBuf + CMD_LEN + PEC_LEN;
    if (HAL_SPI_Receive(hspi, rx_dataBuf, (DATA_LEN + PEC_LEN)*NUM_CHIPS, SPI_TIME_OUT) != HAL_OK)
    {
        // TODO: do something if fails
    }

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
}