#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "adbms_cmd.h"
#include "bms_system_prams.h"

// TODO: Change this value
#define SPI_TIME_OUT HAL_MAX_DELAY  /* SPI Time out delay */
typedef struct
{
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *csb_pinBank;
    uint16_t csb_pin;

    uint8_t spi_tx_dataBuf[DATABUF_LEN];
    uint8_t spi_rx_dataBuf[DATABUF_LEN];

    // Config groups a,b
    uint8_t cfg_a[NUM_CHIPS * DATA_LEN];
    uint8_t cfg_b[NUM_CHIPS * DATA_LEN];

    // AD Commands 
    uint16_t adcv;
    uint16_t adsv;
    uint16_t adax;
    uint16_t adax2;

    uint8_t cell[CELL_REG_GRP * NUM_CHIPS * DATA_LEN];
    uint8_t scell[CELL_REG_GRP * NUM_CHIPS * DATA_LEN];
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

typedef struct
{
  uint8_t rd    :1;
  uint8_t cont  :1;
  uint8_t dcp   :1;
  uint8_t rstf  :1;
  uint8_t ow    :2;
}adcv_;

typedef struct
{
  uint8_t cont  :1;
  uint8_t dcp   :1;
  uint8_t ow    :2;
}adsv_;

typedef struct
{
  uint8_t ow    :1;
  uint8_t pup   :1;
  uint8_t ch    :5;
}adax_;

typedef struct
{
  uint8_t ch    :4;
}adax2_;

uint16_t Pec15_Calc(uint8_t len, uint8_t *data);
uint16_t Pec10_Calc(bool isRxCmd, int len, uint8_t *data);

uint16_t Set_UnderOver_Voltage_Threshold(float voltage);
float ADBMS_getVoltage(int data);

void ADBMS_Init(adbms6830_ICs *ICs, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csb_pinBank, uint16_t csb_pin);

void ADBMS_Set_Config_A(cfa_ *cfg_a, uint8_t *cfg_a_tx_buffer);
void ADBMS_Set_Config_B(cfb_ *cfg_b, uint8_t *cfg_b_tx_buffer);
void ADBMS_Set_ADCV(adcv_ adcv, uint16_t *adcv_cmd_buffer);
void ADBMS_Set_ADSV(adsv_ adsv, uint16_t *adsv_cmd_buffer);
void ADBMS_Set_ADAX(adax_ adax, uint16_t *adax_cmd_buffer);
void ADBMS_Set_ADAX2(adax2_ adax2, uint16_t *adax2_cmd_buffer);

ADBMS_Pack_CMD(uint16_t tx_cmd, uint8_t *spi_tx_dataBuf);
ADBMS_Pack_Write_Data_RegGrp(uint16_t tx_cmd, uint8_t *data, uint8_t *spi_tx_dataBuf);
bool ADBMS_Process_Read_Data_RegGrp(uint8_t *rx_dataBuf, uint8_t *dataBuf);

void ADBMS_WakeUP_ICs_Polling();
void ADBMS_Write_CMD_Polling(SPI_HandleTypeDef *hspi, uint16_t tx_cmd);
void ADBMS_Write_Data_RegGrp_Polling(SPI_HandleTypeDef *hspi, uint16_t tx_cmd, uint8_t *data, uint8_t *spi_tx_dataBuf);
bool ADBMS_Read_Data_RegGrp_Polling(SPI_HandleTypeDef *hspi, uint16_t tx_cmd, uint8_t *data, uint8_t *spi_rx_dataBuf);
//bool ADBMS_Read_Data_Regs_Polling(SPI_HandleTypeDef *hspi, uint8_t num_regs, uint16_t *tx_cmd, uint8_t *dataBuf, uint8_t *spi_dataBuf);

bool ADBMS_TransmitReceive_Reg_DMA(adbms6830_ICs *ICs);
