/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : Button_Test.c
* Author             : MCD Application Team
* Version            : V1.0.0
* Date               : 12/17/2008
* Description        : Button_Test program body
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "includes.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DAC_DHR12R1_Address      0x40007408
#define DAC_DHR12L1_Address      0x4000740C
#define DAC_DHR12L2_Address      0x40007418

#define DAC_DHR12LD_Address      0x40007424

#define  User   6
#define  NOKEY  0

#define BUFFER_SIZE   40000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

void ADC_Init_func(void);
void DAC_Init_func(void);
void NVIC_Conf(void);
void LED_Display_ALL(u8 Ledstatus);

void RCC_Configuration(void);
void GPIO_Configuration(void);
void update_adc1_value(void);

void Start_Recording(void);
void Stop_Recording(void);
void Start_Playing(void);
void Stop_Playing(void);

u8 ReadKey(void);
extern void DelayResolution100us(u32 Delay);
  u8 TxBuffer[BUFFER_SIZE];
  u32 Buffer_index=0;
  u16 ADCConvertedValue1;
  u8 lastKey;
/*******************************************************************************
* Function Name  : Button_Test
* Description    : Button_Test program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Sounds_Test(void)
{
  u32 Record_Length;
  /* Disable EXTI for the menu navigation keys  */
  //IntExtOnOffConfig(DISABLE);

  /* Configuration */
  RCC_Configuration();
  GPIO_Configuration();
  NVIC_Conf();
  ADC_Init_func();
  DAC_Init_func();

  /* Display Test name */
  GLCD_print ("\f");
  //GLCD_SetFont(&Terminal_9_12_6,0xA00,0xFFF);
  GLCD_TextSetPos(6,1);
  GLCD_print( "Audio Test\n ");

  /* turn off all leds */
  LED_Display_ALL(0);

  GLCD_TextSetPos(1,1);
  GLCD_print( "Please connect your\n ");
  GLCD_TextSetPos(2,4);
  GLCD_print( "Headphone and Mic\n ");

  //GLCD_SetFont(&Terminal_9_12_6,0x80,0xFFF);
  GLCD_TextSetPos(0,8);
  GLCD_print("Press User key to     \n ");
  GLCD_TextSetPos(0,9);
  GLCD_print( "record 5s of speech \n ");
  while(ReadKey() != User);

  /* Recording */
  Start_Recording();

  GLCD_TextSetPos(0,9);
  GLCD_print( "play the record.\n ");
  Buffer_index=0;

  DelayResolution100us(1000);
  while(ReadKey() != User)
  {
    if ((Buffer_index<BUFFER_SIZE) && (ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)!=RESET))
    {
      ADCConvertedValue1 = ADC_GetConversionValue(ADC1);
      TxBuffer[Buffer_index++]=(u8)(ADCConvertedValue1>>4 & 0x00FF);
      *(u32*)(DAC_DHR12L2_Address)=ADCConvertedValue1;
      *(u32*)(DAC_DHR12L1_Address)=ADCConvertedValue1;
      //*(u32*)(DAC_DHR12LD_Address)=ADCConvertedValue1<<8 |ADCConvertedValue1;
      ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
    }
  }
  /* Store the record length */
  Record_Length=Buffer_index;
  Buffer_index=0;

  //GLCD_SetFont(&Terminal_9_12_6,0x80,0xFFF);
  GLCD_TextSetPos(0,8);
  GLCD_print( "                      \n ");
  GLCD_TextSetPos(0,1);
  GLCD_print( "Press User key to exit\n ");


  /* Play Record */
  Start_Playing();
  DelayResolution100us(1000);
  while((ReadKey() != User) && (Buffer_index<Record_Length))
  {
    if (ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)!=RESET)
    {
      *(u32*)(DAC_DHR12L2_Address)=TxBuffer[Buffer_index]<<4;
      *(u32*)(DAC_DHR12L1_Address)=TxBuffer[Buffer_index++]<<4;
      if (Buffer_index>Record_Length) Buffer_index=0;
      ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
    }
  }

  Stop_Playing();
  Stop_Recording();
  //ReturnFunc();
  /* Enable EXTI for the menu navigation keys  */
  //IntExtOnOffConfig(ENABLE);

   }

/**
  * @brief  Store ADC1 data in buffer.
  * @param  None
  * @retval : None
  */
void update_adc1_value(void)
{
  if (Buffer_index<BUFFER_SIZE)
  {
    ADCConvertedValue1 = ADC_GetConversionValue(ADC1);//-0x8000;
    TxBuffer[Buffer_index++]=(u8)(ADCConvertedValue1>>4 & 0x00FF);
  }

}


/**
  * @brief  DAC initialisation.
  * @param  None
  * @retval : None
  */
void DAC_Init_func(void )
{

  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  DAC_InitTypeDef            DAC_InitStructure;
  DMA_InitTypeDef            DMA_InitStructure;

  /* TIM6 Configuration */
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 9000;
  TIM_TimeBaseStructure.TIM_Prescaler =0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

  /* TIM7 Configuration */
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 9000;
  TIM_TimeBaseStructure.TIM_Prescaler =0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

  /* TIM7 TRGO selection */
  TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);

  /* DMA2 channel3 configuration */
  DMA_DeInit(DMA2_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12L1_Address;//DAC_DHR12RD_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)TxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize =BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable ;
  DMA_InitStructure.DMA_MemoryInc =DMA_MemoryInc_Enable ;
  DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_HalfWord ;
  DMA_InitStructure.DMA_MemoryDataSize =  DMA_MemoryDataSize_Byte ;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA2_Channel3, &DMA_InitStructure);

  /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* DMA2 channel3 configuration */
 /* DMA_DeInit(DMA2_Channel4);
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12LD_Address;//DAC_DHR12R1_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)TxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize =BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable ;
  DMA_InitStructure.DMA_MemoryInc =DMA_MemoryInc_Enable ;
  DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte ;
  DMA_InitStructure.DMA_MemoryDataSize =  DMA_MemoryDataSize_Byte ;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA2_Channel4, &DMA_InitStructure);*/

  /* DAC channel1 Configuration */
  /*DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;*/
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* Enable DAC Channel1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);

  /* Enable DAC Channel2 */
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* Enable DMA for DAC channel1 */
  DAC_DMACmd(DAC_Channel_1, ENABLE);

  /* Enable DMA for DAC channel2 */
  DAC_DMACmd(DAC_Channel_2, ENABLE);

  /* TIM8 enable counter */
  TIM_Cmd(TIM6, ENABLE);

  /* TIM8 enable counter */
  TIM_Cmd(TIM7, ENABLE);

}

/**
  * @brief  ADC1 initialisation.
  * @param  None
  * @retval : None
  */
void ADC_Init_func(void )
{
  ADC_InitTypeDef	     ADC_InitStructure;
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  /* TIM3 Configuration */
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 9000;
  TIM_TimeBaseStructure.TIM_Prescaler =0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* TIM3 TRGO selection */
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;//ADC_ExternalTrigConv_None;//
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel6 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_55Cycles5);

  /* Disable ADC1 DMA */
  ADC_DMACmd(ADC1, DISABLE);

  /* Enable EOC interupt */
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */
  ADC_ResetCalibration(ADC1);

  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);

  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval : None
  */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure PC.06 (ADC Channel6) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure DAC channe1 & channel2 outputs pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval : None
  */
void RCC_Configuration(void)
{

/* Enable peripheral clocks --------------------------------------------------*/
  /* Enable GPIO and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG
| RCC_APB2Periph_AFIO, ENABLE);

  /* DMA1 and DMA2 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);

  /* Enable ADC1  clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);

   /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  /* TIM8 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

  /* TIM3 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM7 | RCC_APB1Periph_TIM6, ENABLE);
}

/**
  * @brief  Configures NVIC and Vector Table base location.
  * @param  None
  * @retval : None
  */
void NVIC_Conf(void)
{
//  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure and enable ADC interrupt */
  /*NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0 ;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);*/

 /* Configure and enable DMA interrupt */
/*  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);*/

}
/**
  * @brief  Turn ON/OFF the dedicate led
  * @param Ledstatus: Led number from 0 to 3
  * @retval : None
  */

void LED_Display_ALL(u8 Ledstatus)
{
  /* turn off all leds*/
  GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_RESET);
  GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_RESET);
  GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
  GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);

  switch(Ledstatus)
  {
   case(1): GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_SET);
            break;

   case(2): GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_SET);
            break;

   case(3): GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
            break;

   case(4): GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);
            break;
  default:
        break;
   }
}

void Start_Recording()
{
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  LED_Display_ALL(2);
}
void Stop_Recording()
{
  ADC_ITConfig(ADC1, ADC_IT_EOC,DISABLE);	
  LED_Display_ALL(0);
}
void Start_Playing()
{
  DMA_ClearFlag(DMA2_FLAG_TC3);	
  DMA_Cmd(DMA2_Channel3, ENABLE);
  LED_Display_ALL(1);
}
void Stop_Playing()
{
  DMA_Cmd(DMA2_Channel3, DISABLE);
  DMA_ClearFlag(DMA2_FLAG_TC3);	
  LED_Display_ALL(0);

}

u8 ReadKey(void)
{
  u8 newKey=NOKEY;
    if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1))
  {
    newKey=User;
  } 
  if (newKey!=lastKey)
  {
    lastKey=newKey;
    return newKey;
  } else {
    return NOKEY;
  }
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
