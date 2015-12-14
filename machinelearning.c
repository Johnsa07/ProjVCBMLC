#include "includes.h"

int currentState;
char desiredState;
int nextState;
double gammah = 0.7;
bool actionsTested[7] = {false, false, false, false, false, false, false};
int numOfStates = 7;
int roomSize = 15;
int currentPlace;
int carY;
int carX;
int X_accFeedback[8], X_vel[8];
int Y_accFeedback[8], Y_vel[8];

int Q[7][7];
int R[7][7] =
{{100, 0, 0, 0, 0, 0, 0}, //Forward
{0, 100, 0, 0, 0, 0, 0},  //Forward Left
{0, 0, 100, 0, 0, 0, 0}, // Forward Right
{0, 0, 0, 100, 0, 0, 0},  //Still
{0, 0, 0, 0, 100, 0, 0}, // Back Left
{0, 0, 0, 0, 0, 100, 0}, // Back Right
{0, 0, 0, 0, 0, 0, 100}}; //Backward

int QCollision[7][7];
int RCollision[7][7] =
{{0, 100, 100, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 100, 0},
{0, 0, 0, 100, 0, 0, 0},
{0, 0, 0, 100, 0, 0, 0},
{0, 100, 0, 0, 0, 0, 0},
{0, 0, 100, 0, 0, 0, 0},
{0, 0, 0, 0, 100, 100, 0}};

int room[15][15];

void initialize();
int calcQ(int action);
int directReward(int action);
int maxNextQ(int state);
void testAllActions(int state);
int correctAction(int state);
int goToState(int state);
bool validTest(int state);
//bool collisionCheck(int action);


void testAllActions(int state){
  //Redo this method to test actions on the car
  //And wait after each issued action
  	//doAction 0
  car_state car_fb;
        for (int i = 0; i < numOfStates; i++){
          if (i==0 || i==3 || i==6)
          {
                GoCars(i);
        //wait
                if(i==3)
                {
                DWT_Delay(5000000);
                } else {
                  DWT_Delay(1000000);
                }
        //get Accelerometer value
                do
                {
                car_fb = accl_feedback();
                } while (car_fb == car_error);
                Q[state][i] = calcQ(car_fb);
          }
                X_accFeedback[i] = get_X();
                Y_accFeedback[i] = get_Y();
                X_vel[i] = get_Xvel();
                Y_vel[i] = get_intdir();
        }

}

int goToState(int state){
        
        nextState = state;
          
	if (actionsTested[state] == false)
	{
		testAllActions(state);
                if (validTest(state) == true){
                        actionsTested[state] = true;
                }
	}
	currentState = correctAction(state);
        return currentState;
}

bool validTest(int state){
        bool valid = false;
        int test = 0;
        for (int i = 0; i < numOfStates; i++){
                if (Q[state][i] == 100) {
                        test++;
                }                        
        }
        if (test == 1){
                valid = true;
        }
        return valid;
}

int correctAction(int state){
	int bestAction = 0;
	int max = 0;
	for (int i = 0; i < numOfStates; i++){
		if (Q[state][i] > max)
		{
			max = Q[state][i];
			bestAction = i;
		}
	}
	return bestAction;
}

int directReward(int action){
	return R[nextState][action];
}

int maxNextQ(int action){
	int currentMax = 0;
	for (int i = 0; i < numOfStates; i++){
		if (Q[nextState][i] > currentMax){
			currentMax = Q[nextState][i];
		}
	}
	if (currentMax <= directReward(action)){
		currentMax = 0;
	}
	return currentMax;
}

int calcQ(int action){
	return directReward(action) + gammah*maxNextQ(action);
}
                
int Q_get(int Q_index1, int Q_index2)
  {return Q[Q_index1][Q_index2];}


void initialize(){
	srand (time(NULL));
	for (int i = 0; i < numOfStates; i++){
		for (int j = 0; j < numOfStates; j++){
			Q[i][j] = 0;
			//QCollision[i][j] = 0;
		}
	}
}

int get_X_accFeedback(int index) {return X_accFeedback[index];}
int get_Y_accFeedback(int index) {return Y_accFeedback[index];}
int get_X_vel(int index) {return X_vel[index];}
int get_Y_vel(int index) {return Y_vel[index];}

void testExp(){
  //Redo this method to test actions on the car
  //And wait after each issued action
  	//doAction 0
/*   GoCars(car_fw);
   DWT_Delay(150000);
                X_accFeedback[0] = get_X();
                Y_accFeedback[0] = get_Y();
                X_vel[0] = get_Xvel();
                Y_vel[0] = get_Yvel();
                DWT_Delay(150000);
                                X_accFeedback[1] = get_X();
                Y_accFeedback[1] = get_Y();
                X_vel[1] = get_Xvel();
                Y_vel[1] = get_Yvel(); */
                   GoCars(car_backL);
                   DWT_Delay(400000);
        for (int i = 0; i < 8; i++){
                  DWT_Delay(170000);
        //get Accelerometer value

                //X_accFeedback[i] = get_X();
                Y_accFeedback[i] = get_Y();
                //X_vel[i] = get_Xvel();
                Y_vel[i] = get_Yvel();
                X_vel[i] = get_intdir();
                X_accFeedback[i] = get_sumdir();
                if (i==3) {GoCars(car_back);
                DWT_Delay(400000);}
        }
        	GoCars(3);
	
}