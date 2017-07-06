
#include "Module_Slave_I2C.h"
#include "main.h"
#include "constant.h"
#include "Module_MemMap.h"
#include "utilities.h"

static void User_I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c);
static void User_I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c);
static void User_I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c);
static void User_I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c);
static void User_I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c);
static void User_I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c);
static void User_I2C_Slave_AF(I2C_HandleTypeDef *hi2c);

static uint8_t TxRxBuffer[RW_CHUNK_SIZE+1] = {0};                      //数据发送接收缓冲区
static uint8_t Buffer_Internal_Address = 0;                            //缓冲区指针                                                   


void I2C_Slave_Transreceiver_IT_Iniitialize(I2C_HandleTypeDef *hi2c)    //初始化I2C接收
{
    //失能 POS
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);
    
	  //将状态读入I2C结构
    hi2c->State = HAL_I2C_STATE_BUSY;
    hi2c->Mode = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    
	  //将缓冲区和缓冲大小读入I2C结构
    hi2c->pBuffPtr = TxRxBuffer;
    hi2c->XferSize = RW_CHUNK_SIZE+1;
    hi2c->XferCount = RW_CHUNK_SIZE+1;

    //使能响应
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

    //使能中断
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
}

void USER_SLAVE_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c)      //事件中断处理
{
    uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0;
    tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
    tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, (I2C_IT_EVT));
    tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF);
    tmp4 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA);
    /* ADDR 触发 --------------------------------------------------------------*/
    if((tmp1 == SET) && (tmp2 == SET))
    {
      User_I2C_Slave_ADDR(hi2c);
    }
    /* STOPF 触发 --------------------------------------------------------------*/
    else if((tmp3 == SET) && (tmp2 == SET))
    {
      User_I2C_Slave_STOPF(hi2c);
    }
    /* I2C 在 发送状态 -----------------------------------------------*/
    else if(tmp4 == SET)
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
      /* TXE 触发 和 BTF 未触发 -----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        User_I2C_SlaveTransmit_TXE(hi2c);
      }
      /* BTF 触发 -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        User_I2C_SlaveTransmit_BTF(hi2c);
      }
    }
    /* I2C 在 接受状态  --------------------------------------------------*/
    else
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
      /* RXNE 触发 和 BTF 未触发 ----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        User_I2C_SlaveReceive_RXNE(hi2c);
      }
      /* BTF 触发 -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        User_I2C_SlaveReceive_BTF(hi2c);
      }
    }
}
static void User_I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c)        //处理 ADDR FLAG
{
	uint32_t tmp4 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA);
	if((tmp4 == SET) && (hi2c->XferCount < hi2c->XferSize))
	{
		int i = 0;
		Buffer_Internal_Address = (*((hi2c->pBuffPtr)-1));
	  for(i = 0; i < RW_CHUNK_SIZE; i++)
	  {
		  *((hi2c->pBuffPtr)+i) = Read_MemMap(Buffer_Internal_Address + i); 
	  }
  }
  else if ((tmp4 == SET) && (hi2c->XferCount == hi2c->XferSize))
	{
		int j = 0;
		for(j = 0; j < RW_CHUNK_SIZE; j++)
	  {
		  *((hi2c->pBuffPtr)+j) = Read_MemMap(Buffer_Internal_Address + j); 
	  }
	}
  /* Clear ADDR flag */
  __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
	return;
}


static void User_I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c)                //处理STOPF FLAG
{
  /* Disable EVT, BUF and ERR interrupt */
  //__HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
  /* Clear STOPF flag */
  __HAL_I2C_CLEAR_STOPFLAG(hi2c);
  /* Disable Acknowledge */
  // CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);
  // hi2c->State = HAL_I2C_STATE_READY;
   int m = 0;
   int n = PAGE_SIZE + 1 - hi2c->XferCount;
	 if(n >0)
	 {
     Buffer_Internal_Address = TxRxBuffer[0];
	 }
	 while(m+1<n)
	 {
		 Write_MemMap(Buffer_Internal_Address+m, TxRxBuffer[m+1]);
		 m++;
	 }
	 EmptyBuffer(TxRxBuffer);
	 hi2c->pBuffPtr = TxRxBuffer;
   hi2c->XferCount = RW_CHUNK_SIZE+1;
   return;
}
static void User_I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c)         //处理 TXE FLAG
{
  if(hi2c->XferCount != 0)
  {
    /* Write data to DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return;
}
static void User_I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c)         //处理 BTF FLAG （Tx）
{
  if(hi2c->XferCount != 0)
  {
    /* Write data to DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return;
}
static void User_I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c)         //处理 RXNE FLAG
{
  if(hi2c->XferCount != 0)
  {
    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return;
}
static void User_I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c)           //处理 BTF FLAG （Rx）
{
  if(hi2c->XferCount != 0)
  {
    /* Read data from DR */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return;
}
void User_HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c)                 //错误中断处理
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
static void User_I2C_Slave_AF(I2C_HandleTypeDef *hi2c)                       //处理 AF FlAG
{
  /* Disable EVT, BUF and ERR interrupt */
  //__HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);

  /* Clear AF flag */
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

  /* Disable Acknowledge */
  //CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

  //hi2c->State = HAL_I2C_STATE_READY;
	
  EmptyBuffer(TxRxBuffer);
  hi2c->pBuffPtr = TxRxBuffer;
  hi2c->XferCount = RW_CHUNK_SIZE+1;
  return;
}
