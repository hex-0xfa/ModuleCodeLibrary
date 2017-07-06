
#include "Module_MemMap.h"
#include "Module_Initialization.h"

void Module_Init(void)
{
	Internal_Write_MemMap(0,0x11);     //identifier   QSFP28
	Internal_Write_MemMap(1,0x00);     //Revision not specified
	Internal_Write_MemMap(2,0x07);     //IntL 高， 无多 Page， Data Not Ready 高
	//在MASTERI2C或者MASTERMONITER中需要这些函数：
	//GetLOS()        -> 3
	//GetTxFault()    -> 4
  //GetLOL()        -> 5
  // 6 温度预警（可选）A/W
	// 7 电压预警（可选）A/W
	// 8 随意
	// 9-10 功率预警 （可选）A/W
	// 11-12 TX偏置电流预警 （可选） A/W
	// 13-14 TX PWR 预警 （可选） A/W
	// 15-18 保留位
	// 19-21 随意
	//在utilities中需要这些函数
  //GetTemperature() -> 22,23
	//GetSupplyVoltage() -> 26,27
	// 28-29 保留位
	// 30-33 随意
	// 34-41 Rx Power MASTERI2C: GetRxPower()
	// 42-49 Tx Bias  MASTERI2C: GetTxBias()
	// 50-57 Tx PWR   MASTERI2C: GetTxPWR()
	// 58-65 保留位
	// 66-81 随意
	// 82-85 保留位
}

