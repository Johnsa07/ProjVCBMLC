#define HCSR04_PORT   GPIOE
#define HCSR04_TRIG   GPIO_Pin_6
#define HCSR04_ECHO    GPIO_Pin_8

#include "includes.h"


void HCSR04_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

    //TRIG init
  GPIO_InitStructure.GPIO_Pin =  HCSR04_TRIG;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);

////LED
//  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
//GPIO_Init(GPIOC, &GPIO_InitStructure);
  
      //ECHO init
  GPIO_InitStructure.GPIO_Pin =  HCSR04_ECHO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
  
  GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);


  }

uint32_t HCSR04_Read(void)
{
  uint32_t start, stop;
  GPIO_SetBits(HCSR04_PORT, HCSR04_TRIG);
  DWT_Delay(10);
  GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);
  
  while (!GPIO_ReadInputDataBit(HCSR04_PORT, HCSR04_ECHO));

  start=DWT_Get();
  while (GPIO_ReadInputDataBit(HCSR04_PORT, HCSR04_ECHO));
  

  stop=DWT_Get();
  return ((stop-start)/72);  
}

         
//void Led_On(void)
//{
//  GPIO_SetBits(GPIOC, GPIO_Pin_6); //LED on
//}
//
//void Led_Off(void)
//{
//  GPIO_ResetBits(GPIOC, GPIO_Pin_6); //LED on
//}