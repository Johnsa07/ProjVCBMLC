/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : main.c
 *    Description : Define main module
 *
 *    History :
 *    1. Date        : 19, July 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  This example project shows how to use the IAR Embedded Workbench
 * for ARM to develop code for the IAR-STM32F107VC-SK board. It shows basic
 * use of parallel I/O, timer, interrupt controller, interface to
 * a Nokia Graphic color LCD module, step motor and 3D accelerometer sensor.
 *  It try to lock position motor's rotor pointer regardless of the board
 * position.
 *
 *  Jumpers:
 *   PWR_SEL - depending of power source
 *   JTAG_SEL  - open (on board j-link)
 *   B0_1/B0_0 - B0_0
 *   B1_1/B1_0 - B1_0
 *
 *    $Revision: 39 $
 **************************************************************************/

//Hej!!

#include "includes.h"

#define DLY_100US  450

extern FontType_t Terminal_6_8_6;
extern FontType_t Terminal_9_12_6;
extern FontType_t Terminal_18_24_12;

Int32U CriticalSecCntr;
car_state car_feedback, car_instr=car_stop, action=0;
Boolean NewData = FALSE;
Boolean Dir;
Int32U Steps;

volatile Boolean SysTickF1= TRUE;
volatile Boolean NewInstr = TRUE;
volatile Boolean Dummy = FALSE;
volatile int Test = 0;
volatile int TestTurn= 0;
car_state whichState(void);
void position(void);

/*************************************************************************
 * Function Name: TickHandler
 * Parameters: void
 * Return: void
 *
 * Description:
 *		
 *************************************************************************/
void TickHandler(void)
{
SysTickF1=TRUE;
position();
if (car_instr == car_stop && !NewInstr)
{
    if(!GPIO_ReadInputDataBit(JS_RIGHT_PORT, JS_RIGHT_MASK)) //!(JS_RIGHT_MASK & JS_RIGHT_PORT->IDR)
  {
      NewInstr = TRUE;
      car_instr=car_stop;
  }
  /*else if (!GPIO_ReadInputDataBit(JS_LEFT_PORT, JS_LEFT_MASK))  //!(JS_LEFT_MASK & JS_LEFT_PORT->IDR)
  {
      NewInstr = TRUE;
      car_instr=car_fwR;
  }*/
    else if (!(JS_DOWN_MASK & JS_DOWN_PORT->IDR))
  {
      NewInstr = TRUE;
      car_instr=car_back;
  }
    else if (!(JS_UP_MASK & JS_UP_PORT->IDR))
  {
      NewInstr = TRUE;
      car_instr=car_fw;
  } 
}
}

/*************************************************************************
 * Function Name: DelayResolution100us
 * Parameters: Int32U Dly
 *
 * Return: none
 *
 * Description: Delay ~ (arg * 100us)
 *
 *************************************************************************/
void DelayResolution100us(Int32U Dly)
{
  for(; Dly; Dly--)
  {
    for(volatile Int32U j = DLY_100US; j; j--)
    {
    }
  }
}

/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: main
 *
 *************************************************************************/
void move_end_check(void);
void data_transfer(void);

 void main(void)
{

  ENTR_CRT_SECTION();
  /* Setup STM32 system (clock, PLL and Flash configuration) */
  SystemInit();

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  


  // I2C1 init
  I2C1_Init();

  EXT_CRT_SECTION();

  // GLCD init
  GLCD_PowerUpInit(0x0); //(
  GLCD_Backlight(BACKLIGHT_ON);
  GLCD_SetFont(&Terminal_9_12_6,0x000F00,0x00FF0);
  GLCD_SetWindow(10,10,131,131);

  // Init Accl sensor
  if(FALSE == Accl_Init())
  {
    // Initialization fault
    GLCD_TextSetPos(0,0);
    GLCD_print("\fLIS3LV020 Init.\r\nfault\r\n");
    while(1);
  }
    //Init CarControl and Delay
  Car_Init();
  DWT_Init();
  HCSR04_Init();
    // SysTick end of count event each 0.5s with input clock equal to 9MHz (HCLK/8, default)
  SysTick_Config(150000);
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

  DWT_Delayms(1000);
  while(1)
  {

    //while(1){GLCD_print("Current state: %d \r",GPIO_ReadInputDataBit(JS_LEFT_PORT, JS_LEFT_MASK));} 
    //car_feedback=accl_feedback();
 /*   A = accX[1];
    B = velX[1];
    C = posX[1]; */
if(SysTickF1)
{
        SysTickF1 = FALSE;
        //GLCD_TextSetPos(0,0);
            GLCD_print("%d, %d \r", get_Xvel(), accl_feedback());
            DWT_Delayms(500);
            
}
    if(NewInstr) //
    {
      DWT_Delayms(1000);
      NewInstr = FALSE;
    
    //GoCar(Test, TestTurn);
    // 1. Give command (desired state)
    //desiredState;

    // 2. Run machine learning to test action
    action = goToState(car_instr);
    GoCars(action);
    //  testExp();
    int runTime = 0;
    while (runTime < 1){
        DWT_Delayms(700);
        runTime++;
    }
            GLCD_TextSetPos(0,0);
            GLCD_print("\f%d,%d;%d,%d\r\n", get_X_accFeedback(0), get_Y_accFeedback(0), get_X_vel(0), get_Y_vel(0));
            GLCD_print("%d,%d;%d,%d\r\n", get_X_accFeedback(1), get_Y_accFeedback(1), get_X_vel(1), get_Y_vel(1));
            GLCD_print("%d,%d;%d,%d\r\n", get_X_accFeedback(2), get_Y_accFeedback(2), get_X_vel(2), get_Y_vel(2));
            GLCD_print("%d,%d;%d,%d\r\n", get_X_accFeedback(3), get_Y_accFeedback(3), get_X_vel(3), get_Y_vel(3));
            GLCD_print("%d,%d;%d,%d\r\n", get_X_accFeedback(4), get_Y_accFeedback(4), get_X_vel(4), get_Y_vel(4));
            GLCD_print("%d,%d;%d,%d\r\n", get_X_accFeedback(5), get_Y_accFeedback(5), get_X_vel(5), get_Y_vel(5));
            GLCD_print("%d,%d;%d,%d\r\n", get_X_accFeedback(6), get_Y_accFeedback(6), get_X_vel(6), get_Y_vel(6));
      //      GLCD_print("%d,%d;%d,%d\r\n", get_X_accFeedback(7), get_Y_accFeedback(7), get_X_vel(7), get_Y_vel(7));
                car_instr=car_stop;
    GoCars(car_instr);
            DWT_Delayms(2000);
    }
    //GoCars(3); //Stopping the car
    
    
    // 3. Wait x ms
    //Wait in ML-method testAllActions
    
    // 4. Return value from accelerometer
    //Return value from accelerometer in ML-method testAllActions
    
    // 5. Evaluate action compared to state
    //Already implemented in ML-file. No need to alter (probably)
    
    // 6. Repeat 3-5 until all actions has been tested
    //Already implemented in ML-file. No need to alter (probably)
    
    // 7. Choose the correct state
    //Already implemented in ML-file. No need to alter (probably)
  }
}

car_state whichState(void)
{
  if (Test==0) {return car_stop;}
  else if (Test==1)
  {
    if (TestTurn==0) {return car_fw;}
    else if (TestTurn==1) {return car_fwR;}
    else if (TestTurn==2) {return car_fwL;}
  } 
  else if (Test==2)
  {
    if (TestTurn==0) {return car_back;}
    else if (TestTurn==1) {return car_backR;}
    else if (TestTurn==2) {return car_backL;}
  }
  return car_error;
}




/*void data_transfer(void)
{
  signed long positionXbkp, positionYbkp;
  unsigned int delay;
  unsigned char posx_seg[4], posy_Sef[4];
  
  if (positionX[1] >= 0) //Compares the sign of the X direction data
  {
    direction = (direction | 0x10); // if it's positive the most significant byte 
    posx_seg[0] = positionX[1] & 0x000000FF; // is set to 1 else it is set to 8
    posx_seg[1] = (positionX[1] >> 8) & 0x000000FF; //the data is also managed in the
    // subsequent lines in order to
    posx_seg[2] = (positionX[1] >> 16) & 0x000000FF; // be sent. The 32 bit variable must be
    posx_seg[3] = (positionX[1] >> 24) & 0x000000FF; // split into 4 different 8 bit
    // variables in order to be sent via the 8 bit SCI frame

  }
  else
  {
    direction = (direction | 0x80);
  }
    
} */
