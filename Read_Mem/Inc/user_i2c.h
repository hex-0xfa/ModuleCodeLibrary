#include "stm32f1xx_hal.h"

HAL_StatusTypeDef HAL_I2C_Slave_Bimode_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
static HAL_StatusTypeDef User_I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout);
void USER_SlAVE_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);
void User_HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef User_I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef User_I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef User_I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef User_I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef User_I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef User_I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef User_I2C_Slave_AF(I2C_HandleTypeDef *hi2c);
