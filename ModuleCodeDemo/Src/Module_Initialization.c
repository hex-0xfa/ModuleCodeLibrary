
#include "Module_MemMap.h"
#include "Module_Initialization.h"

void Module_Init(void)
{
	Internal_Write_MemMap(0,0x11);     //identifier   QSFP28
	Internal_Write_MemMap(1,0x00);     //Revision not specified
	Internal_Write_MemMap(2,0x07);     //IntL �ߣ� �޶� Page�� Data Not Ready ��
	//��MASTERI2C����MASTERMONITER����Ҫ��Щ������
	//GetLOS()        -> 3
	//GetTxFault()    -> 4
  //GetLOL()        -> 5
  // 6 �¶�Ԥ������ѡ��A/W
	// 7 ��ѹԤ������ѡ��A/W
	// 8 ����
	// 9-10 ����Ԥ�� ����ѡ��A/W
	// 11-12 TXƫ�õ���Ԥ�� ����ѡ�� A/W
	// 13-14 TX PWR Ԥ�� ����ѡ�� A/W
	// 15-18 ����λ
	// 19-21 ����
	//��utilities����Ҫ��Щ����
  //GetTemperature() -> 22,23
	//GetSupplyVoltage() -> 26,27
	// 28-29 ����λ
	// 30-33 ����
	// 34-41 Rx Power MASTERI2C: GetRxPower()
	// 42-49 Tx Bias  MASTERI2C: GetTxBias()
	// 50-57 Tx PWR   MASTERI2C: GetTxPWR()
	// 58-65 ����λ
	// 66-81 ����
	// 82-85 ����λ
}

