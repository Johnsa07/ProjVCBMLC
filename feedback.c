
#include "includes.h"

int countX, countY;
Int16S X, Y, Z, XVel=0, YVel=0, ZVel=0 , XPos=0, YPos=0, ZPos=0;
long velX[2], velY[2], accX[2], accY[2], posX[2], posY[2], XXX, YYY;
void move_end_check(void);
car_state XV;
const int sense=80;

 void position(void)
{
  unsigned int count1;
  count1 = 0;
  accX[1] = 0;
  accY[1] = 0;
  do
  {
    Accl_Get(&X,&Y,&Z);
    accX[1] = accX[1] + X;
    accY[1] = accY[1] + Y;
    count1++;
  }while (count1 != 64); // 64 sums of the acceleration sample
    
    accX[1]=accX[1]>>6; //Div by 64
    accY[1]=accY[1]>>6;
    
    //accX[1]=accX[1]-30; //Remove the offset due to gravity
    //accY[1]=accY[1]-6;
    XXX=accX[1];
   YYY=accY[1];

    if ((accX[1] <=sense)&&(accX[1] >= -sense)) //Discrimination window applied
      {accX[1] = 0;} // to the X axis acceleration
      //variable

    if ((accY[1] <= sense)&&(accY[1] >= -sense))
      {accY[1] = 0;} 
    
    //First integration:
    velX[1] = velX[0] + accX[0] + ((accX[1] - accX[0])>>1);
    velY[1] = velY[0] + accY[0] + ((accY[1] - accY[0])>>1);
    
    //Second integration:
    posX[1] = posX[0] + velX[0] + ((velX[1] - velX[0])>>1);
    posY[1] = posY[0] + velY[0] + ((velY[1] - velY[0])>>1);
    
    //Current values sent to previous values
    accX[0] = accX[1];
    accY[0] = accY[1];
    velX[0] = velX[1];
    velY[0] = velY[1];
    
    //posX[1]=posX[1]>>10; //Sensibility adjustment
    posY[1] = posY[1]>>18;
    
    //data_tranfer();
    
    //posX[1]=posX[1]<<18; //Return original value
    posY[1] = posY[1]<<18;    
    
    move_end_check();
    
    posX[0] = posX[1];
    posY[0] = posY[1];
    posX[1]=posX[1]>>16;

/*if (accX[1] == 0)
{return car_stop;}
   else if (accX[1] < 0 && accY[1] == 0)
  {return car_fw;}
   else if (accX[1] > 0 && accY[1] == 0)
  {return car_back;}
   else if (accX[1] < 0 && accY[1] > 0)
  {return car_fwL;}
   else if (accX[1] < 0 && accY[1] < 0)
  {return car_fwR;}
   else if (accX[1] > 0 && accY[1] > 0)
  {return car_backL;}
   else if (accX[1] > 0 && accY[1] < 0)
  {return car_backR;}
  else
  {return car_error;}*/
}

void move_end_check(void)
{
  if (accX[1] == 0) //Count number of acceleration samples equal to zero
  {countX++;}
  else 
  {countX=0;}
  
  if (countX >= 45) //if this number exceeds 25, we assume that veocity i zero
  {
    velX[1]=0;
    velX[0]=0;
  }
  
  if (accY[1]==0) //Same as above...
  {countY++;}
  else {countY=0;}
  
  if (countY >= 45)
  {
    velY[1]=0;
    velY[0]=0;
  }
}

long get_X() {return XXX;}
long get_Y() {return YYY;}
long get_Xvel() {return velX[0];}
long get_Yvel() {return velY[0];}

car_state accl_feedback(void)
{

      if (velX[1] == 0 && velY[1] == 0)
{return car_stop;}
   else if (velX[1] > 0 && velY[1] == 0)
  {return car_fw;}
   else if (velX[1] < 0 && velY[1] == 0)
  {return car_back;}
   else if (velX[1] > 0 && accY[1] > 0)
  {return car_fwL;}
   else if (velX[1] > 0 && accY[1] < 0)
  {return car_fwR;}
   else if (velX[1] < 0 && accY[1] > 0)
  {return car_backL;}
   else if (velX[1] < 0 && accY[1] < 0)
  {return car_backR;}
  else
  {return car_error;}
}
  
