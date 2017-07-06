#include "Module_MemMap.h"
#include "main.h"
#include "constant.h"
#include "stdlib.h"

//1.0 �� ֻ֧�� SFF8636�� ���������ܣ���ȥģ����Ϣ��Memory Map����256���ֽ�
#define RO_START_ADDRESS_1    0
#define RO_STOP_ADDRESS_1    85
#define RO_START_ADDRESS_2  108
#define RO_STOP_ADDRESS_2   111
#define RO_START_ADDRESS_3  128
#define RO_STOP_ADDRESS_3   223
//������ֻ������Ļ���Ҫ�����


static uint8_t MemMap_Array[MEMORY_MAP_SIZE] = {0};                  //�ڲ��ڴ��ַ
//static int Address_Redirection(int Memory_Address);                ʵ�ֶ�ҳ��MemMap��δ��ɣ�

uint8_t* Get_Memmap(void)                                                //��ȡMemMap��ַ
{
	return MemMap_Array;
}

uint8_t Read_MemMap(int Memory_Address)                            //��MemMap
{
	if( Memory_Address >= MEMORY_MAP_SIZE)
	{
		return READ_ERR;
	}
	//*(MemMap_Array + Memory_Address) = MASTERI2C->GetFromChip(Memory_Address)
  return *(MemMap_Array + Memory_Address);
}

uint8_t Write_MemMap(int Memory_Address, uint8_t value)             //дMemMap
{
	if(( Memory_Address >= MEMORY_MAP_SIZE)                                                ||
		 ((Memory_Address >= RO_START_ADDRESS_1) && (Memory_Address <= RO_STOP_ADDRESS_1)) ||
		 ((Memory_Address >= RO_START_ADDRESS_2) && (Memory_Address <= RO_STOP_ADDRESS_2)) ||
	   ((Memory_Address >= RO_START_ADDRESS_3) && (Memory_Address <= RO_STOP_ADDRESS_3)))        
	   //������ֻ������Ļ���Ҫ�����
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

uint8_t Internal_Write_MemMap(int Memory_Address, uint8_t value)     //�ڲ�дMemMap
{
	if(Memory_Address >= MEMORY_MAP_SIZE)
	{
		return WRITE_ERR;
	}
  *(MemMap_Array + Memory_Address) = value;
	return WRITE_NORMAL;
}
//86 Tx ʧ��
//87 ѡ�� Rx ������ ����ѡ��
//88 ѡ�� Tx ������ ����ѡ��
//89 - 92 Software Application ѡ�� ����ѡ��
