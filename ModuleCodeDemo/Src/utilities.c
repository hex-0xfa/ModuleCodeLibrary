#include "utilities.h"
#include "constant.h"
#include "stdint.h"

void EmptyBuffer(uint8_t* a)
{
	int i = 0;
	for(i =0; i<= PAGE_SIZE+1;i++)
	{
		a[i] = 0;
  }
}
