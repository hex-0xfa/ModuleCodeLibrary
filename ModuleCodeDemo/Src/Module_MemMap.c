#include "Module_MemMap.h"
#include "main.h"
#include "constant.h"
#include "stdlib.h"

//1.0 版 只支持 SFF8636中 定义的最简功能，除去模块信息，Memory Map共有256个字节
#define RO_START_ADDRESS_1    0
#define RO_STOP_ADDRESS_1    85
#define RO_START_ADDRESS_2  108
#define RO_STOP_ADDRESS_2   111
#define RO_START_ADDRESS_3  128
#define RO_STOP_ADDRESS_3   223
//如增加只读区域的话需要修添加


static uint8_t MemMap_Array[MEMORY_MAP_SIZE] = {0};                  //内部内存地址
//static int Address_Redirection(int Memory_Address);                实现多页的MemMap（未完成）

uint8_t* Get_Memmap(void)                                                //获取MemMap地址
{
	return MemMap_Array;
}

uint8_t Read_MemMap(int Memory_Address)                            //读MemMap
{
	if( Memory_Address >= MEMORY_MAP_SIZE)
	{
		return READ_ERR;
	}
	//*(MemMap_Array + Memory_Address) = MASTERI2C->GetFromChip(Memory_Address)
  return *(MemMap_Array + Memory_Address);
}

uint8_t Write_MemMap(int Memory_Address, uint8_t value)             //写MemMap
{
	if(( Memory_Address >= MEMORY_MAP_SIZE)                                                ||
		 ((Memory_Address >= RO_START_ADDRESS_1) && (Memory_Address <= RO_STOP_ADDRESS_1)) ||
		 ((Memory_Address >= RO_START_ADDRESS_2) && (Memory_Address <= RO_STOP_ADDRESS_2)) ||
	   ((Memory_Address >= RO_START_ADDRESS_3) && (Memory_Address <= RO_STOP_ADDRESS_3)))        
	   //如增加只读区域的话需要修添加
	{
		return WRITE_ERR;
	}
	else
	{
    *(MemMap_Array + Memory_Address) = value;
		//MASTERI2C->PushToChip(Memory_Address,value)
	}
	return WRITE_NORMAL;
}

uint8_t Internal_Write_MemMap(int Memory_Address, uint8_t value)     //内部写MemMap
{
	if(Memory_Address >= MEMORY_MAP_SIZE)
	{
		return WRITE_ERR;
	}
  *(MemMap_Array + Memory_Address) = value;
	return WRITE_NORMAL;
}
//86 Tx 失能
//87 选择 Rx 的速率 （可选）
//88 选择 Tx 的速率 （可选）
//89 - 92 Software Application 选择 （可选）
