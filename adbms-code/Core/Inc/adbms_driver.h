#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "adbms_cmd.h"
#include "ad_system_prams.h"

// TODO: Change this value
#define SPI_TIME_OUT HAL_MAX_DELAY  /* SPI Time out delay */
typedef struct
{
    SPI_HandleTypeDef *hspi;

    uint8_t spi_dataBuf[DATABUF_LEN];

    // Config groups a,b
    uint8_t cfg_a[NUM_CHIPS * DATA_LEN];
    uint8_t cfg_b[NUM_CHIPS * DATA_LEN];

    // TODO: Figure out a better way to handle these
    uint16_t ADCV;
    uint16_t ADSV;
    uint16_t ADAX; 
    uint16_t ADAX2; 

    uint8_t cell[CELL_REG_GRP * NUM_CHIPS * DATA_LEN];
    uint8_t aux[AUX_REG_GRP * NUM_CHIPS * DATA_LEN];

} adbms6830_ICs;

typedef struct
{
  uint8_t       refon   :1;
  uint8_t       cth     :3;
  uint8_t       flag_d  :8;
  uint8_t       soakon  :1;
  uint8_t       owrng   :1;
  uint8_t       owa     :3;
  uint16_t      gpo     :10;
  uint8_t       snap    :1;
  uint8_t       mute_st :1;
  uint8_t       comm_bk :1;
  uint8_t       fc      :3;
}cfa_;

/* For ADBMS6830 config register structure */
typedef struct
{
  uint16_t 	vuv     :12;
  uint16_t 	vov     :12;
  uint8_t 	dtmen   :1;
  uint8_t 	dtrng   :1;
  uint8_t 	dcto    :6;
  uint16_t 	dcc     :16;
}cfb_;

uint16_t Pec15_Calc(uint8_t len, uint8_t *data);
uint16_t Pec10_Calc(bool isRxCmd, int len, uint8_t *data);

uint16_t Set_UnderOver_Voltage_Threshold(float voltage);

void ADBMS_Set_Config_A(cfa_ *cfg_a, uint8_t *cfg_a_tx_buffer);
void ADBMS_Set_Config_B(cfb_ *cfg_b, uint8_t *cfg_b_tx_buffer);

void ADBMS_WakeUP_ICs();
void ADBMS_Write_CMD(SPI_HandleTypeDef *hspi, uint16_t tx_cmd);
void ADBMS_Write_Data(SPI_HandleTypeDef *hspi, uint16_t tx_cmd, uint8_t *data, uint8_t *spi_dataBuf);
bool ADBMS_Read_Data(SPI_HandleTypeDef *hspi, uint16_t tx_cmd, uint8_t *data, uint8_t *spi_dataBuf);
