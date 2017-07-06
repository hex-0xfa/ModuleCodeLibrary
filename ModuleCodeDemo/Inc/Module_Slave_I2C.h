#ifndef MODULE_SLAVE_I2C
#define MODULE_SLAVE_I2C

#include "stm32f1xx_hal.h"

void I2C_Slave_Transreceiver_IT_Iniitialize(I2C_HandleTypeDef *hi2c);
void USER_SLAVE_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);
void User_HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c);

#endif
