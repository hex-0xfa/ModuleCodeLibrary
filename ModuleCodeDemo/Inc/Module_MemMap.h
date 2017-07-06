
#ifndef MODULE_MEMMAP_H
#define MODULE_MEMMAP_H

#include "main.h"
#include "stdlib.h"

#define WRITE_ERR     1
#define WRITE_NORMAL  0
#define READ_ERR      0


uint8_t* Get_Memmap(void);                                      
uint8_t Read_MemMap(int internal_address);                  
uint8_t Write_MemMap(int internal_address, uint8_t value);
uint8_t Internal_Write_MemMap(int internal_address, uint8_t value);    //只有初始化和与板载芯片沟通时能用
#endif
