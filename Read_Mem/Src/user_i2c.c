#include "user_i2c.h"
#include "main.h"
#include "constant.h"

HAL_StatusTypeDef HAL_I2C_Slave_Bimode_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size)
{
  if(hi2c->State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    if(User_I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, 10000) != HAL_OK)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Disable Pos */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

    hi2c->State = HAL_I2C_STATE_BUSY;
    hi2c->Mode = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

    hi2c->pBuffPtr = pData;
    hi2c->XferSize = Size;
    hi2c->XferCount = Size;

    /* Enable Address Acknowledge */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    /* Note : The I2C interrupts must be enabled after unlocking current process
              to avoid the risk of I2C interrupt handle execution before current
              process unlock */

    /* Enable EVT, BUF and ERR interrupt */
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

static HAL_StatusTypeDef User_I2C_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout)
{
  uint32_t tickstart = 0;

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait until flag is set */
  if(Status == RESET)
  {
    while(__HAL_I2C_GET_FLAG(hi2c, Flag) == RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          hi2c->State= HAL_I2C_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  else
  {
    while(__HAL_I2C_GET_FLAG(hi2c, Flag) != RESET)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          hi2c->State= HAL_I2C_STATE_READY;

          /* Process Unlocked */
          __HAL_UNLOCK(hi2c);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  return HAL_OK;
}

void USER_SlAVE_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c)
{
  uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0;
    tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
    tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, (I2C_IT_EVT));
    tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF);
    tmp4 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA);
    /* ADDR set --------------------------------------------------------------*/
    if((tmp1 == SET) && (tmp2 == SET))
    {
      User_I2C_Slave_ADDR(hi2c);
    }
    /* STOPF set --------------------------------------------------------------*/
    else if((tmp3 == SET) && (tmp2 == SET))
    {
      User_I2C_Slave_STOPF(hi2c);
    }
    /* I2C in mode Transmitter -----------------------------------------------*/
    else if(tmp4 == SET)
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
      /* TXE set and BTF reset -----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        User_I2C_SlaveTransmit_TXE(hi2c);
      }
      /* BTF set -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        User_I2C_SlaveTransmit_BTF(hi2c);
      }
    }
    /* I2C in mode Receiver --------------------------------------------------*/
    else
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
      /* RXNE set and BTF reset ----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        User_I2C_SlaveReceive_RXNE(hi2c);
      }
      /* BTF set -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        User_I2C_SlaveReceive_BTF(hi2c);
      }
    }
}
static HAL_StatusTypeDef User_I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c)
{
	uint32_t tmp4 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA);
	if(tmp4 == SET)
	{
		int i = 0;
		uint8_t Internal_Address = (*((hi2c->pBuffPtr)-1));
		uint8_t* QSFP28_MEMORY = GetQSFPMemoryAddress();
	 for(i = 0; i < PAGE_SIZE; i++)
	 {
		  *((hi2c->pBuffPtr)+i) = QSFP28_MEMORY[Internal_Address+i];
	 }
	}
  /* Clear ADDR flag */
  __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

  return HAL_OK;
}

/**
  * @brief  Handle STOPF flag for Slave Mode
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
static HAL_StatusTypeDef User_I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c)
{
  /* Disable EVT, BUF and ERR interrupt */
  //__HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

  /* Clear STOPF flag */
  __HAL_I2C_CLEAR_STOPFLAG(hi2c);

  /* Disable Acknowledge */
 // CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

 // hi2c->State = HAL_I2C_STATE_READY;

  HAL_I2C_SlaveRxCpltCallback(hi2c);

  return HAL_OK;
}
static HAL_StatusTypeDef User_I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount != 0)
  {
    /* Write data to DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return HAL_OK;
}
static HAL_StatusTypeDef User_I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount != 0)
  {
    /* Write data to DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return HAL_OK;
}
static HAL_StatusTypeDef User_I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount != 0)
  {
    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return HAL_OK;
}
static HAL_StatusTypeDef User_I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->XferCount != 0)
  {
    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return HAL_OK;
}
void User_HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c)
{
  uint32_t tmp1 = 0;
	uint32_t tmp2 = 0;

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BERR);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Bus error interrupt occurred ----------------------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_BERR;

    /* Clear BERR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);
    
    /* Workaround: Start cannot be generated after a misplaced Stop */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_SWRST);
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ARLO);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Arbitration Loss error interrupt occurred ---------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_ARLO;

    /* Clear ARLO flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Acknowledge failure error interrupt occurred ------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    tmp1 = hi2c->Mode;
    if(tmp1 == HAL_I2C_MODE_SLAVE)
    {
      User_I2C_Slave_AF(hi2c);
    }
    else
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
  
      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1,I2C_CR1_STOP);
  
      /* Clear AF flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
    }
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_OVR);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Over-Run/Under-Run interrupt occurred -------------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_OVR;
    /* Clear OVR flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_OVR);
  }

  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    hi2c->State = HAL_I2C_STATE_READY;
    
    /* Disable Pos bit in I2C CR1 when error occured in Master/Mem Receive IT Process */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);
    
    HAL_I2C_ErrorCallback(hi2c);
  }
}
static HAL_StatusTypeDef User_I2C_Slave_AF(I2C_HandleTypeDef *hi2c)
{
  /* Disable EVT, BUF and ERR interrupt */
  //__HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

  /* Clear AF flag */
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

  /* Disable Acknowledge */
  //CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

  //hi2c->State = HAL_I2C_STATE_READY;

  HAL_I2C_SlaveTxCpltCallback(hi2c);

  return HAL_OK;
}
