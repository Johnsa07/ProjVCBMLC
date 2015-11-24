#include "stm32f10x.h"
 
uint32_t SystemCoreClock1=72000000;
 
void DWT_Init(void) 
{
  if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) 
  {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }
}
 
uint32_t DWT_Get(void)
{
  return DWT->CYCCNT;
}
 
uint8_t DWT_Compare(int32_t tp)
{
  return (((int32_t)DWT_Get() - tp) < 0);
}
 
void DWT_Delay(uint32_t us) // microseconds
{
  int32_t tp = DWT_Get() + us * (SystemCoreClock1/1000000);
  while (DWT_Compare(tp));
}

void DWT_Delayms(uint32_t ms) // milliseconds
{
  DWT_Delay(ms*1000);
}