#include "includes.h"

int prevState;
int currentState;
int nextState;
int memState;
double gammah = 0.7;
bool avoid = false;
bool actionsTested[7] = {false, false, false, false, false, false, false};
bool actionsTestedCollision[7] = {false, false, false, false, false, false, false};
int numOfStates = 7;
int X_accFeedback[7], X_vel[7];
int Y_accFeedback[7], Y_vel[7];

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
{{0, 90, 0, -1, -1, -1, -1},
{100, 0, 90, -1, -1, -1, -1},
{0, 100, 0, -1, -1, -1, -1},
{-1, 90, 0, -1, -1, -1, -1},
{-1, 90, 0, -1, -1, -1, -1},
{-1, 90, 0, -1, -1, -1, -1},
{-1, 90, 0, -1, -1, -1, -1}};

void initialize(); //Klar
int calcQ(int action, bool distance);
int directReward(int action, bool distance);
int maxNextQ(int state, bool distance);
void testAllActions(int state, bool distance);
int correctAction(int state, bool distance);
void goToState(int state);
bool validTest(int state);
bool collisionCheck(int action);
bool distanceCheck(int action);


void testAllActions(int state, bool distance){
  //Redo this method to test actions on the car
  //And wait after each issued action
  //doAction 0
  car_state car_fb;
  if (distance == false){
    for (int i = 0; i < numOfStates; i++){
      GoCars(i);
      //wait
      if(i==3)
      {
        DWT_Delay(5000000);
      } else {
        DWT_Delay(200000);
      }
      //get Accelerometer value
      car_fb = accl_feedback();
      X_accFeedback[i] = get_X();
      Y_accFeedback[i] = get_Y();
      X_vel[i] = get_Xvel();
      Y_vel[i] = get_Yvel();
      Q[state][i] = calcQ(car_fb, distance);
    }
    
  }
  else{
    for (int i = 0; i < numOfStates; i++){
      if (RCollision[state][i] != -1){    
        GoCars(i);
        //wait
        if(i==3)
        {
          DWT_Delay(5000000);
        } else {
          DWT_Delay(200000);
        }
        //get Accelerometer value
        car_fb = accl_feedback();
        X_accFeedback[i] = get_X();
        Y_accFeedback[i] = get_Y();
        X_vel[i] = get_Xvel();
        Y_vel[i] = get_Yvel();
        Q[state][i] = calcQ(car_fb, distance);
      }
    }
  }
}

void goToState(int state){
  bool collide;
  int actionCounter = 0;
  collide = distanceCheck(300);
  if (collide == false){
    if (actionsTested[state] == false)
    {
      testAllActions(state, collide);
      actionsTested[state] = true;
    }
    prevState = currentState;
    currentState = correctAction(state, collide);
  }
  else {
    do {
      if (actionsTestedCollision[currentState] == false)
      {
        testAllActions(currentState, collide);
        actionsTestedCollision[currentState] = true;
      }
      memState = prevState;
      prevState = currentState;
      currentState = correctAction(currentState, collide);
      actionCounter++;
    } while (currentState != 0  /*actionCounter < 7*/);
  }
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

int correctAction(int state, bool distance){
  int bestActionMax = 0;
  int bestActionEqual = 0;
  bool equalAction = false;
  int max = 0;
  int equal = 10;
  if (distance == false){
    for (int i = 0; i < numOfStates; i++){
      if (Q[state][i] > max)
      {
        max = Q[state][i];
        bestActionMax = i;
      }
    }
    return bestActionMax;
  }
  else {
    for (int i = 0; i < numOfStates; i++){
      if (QCollision[state][i] > equal && QCollision[state][i] < 100)
      {
        equal = QCollision[state][i];
        bestActionEqual = i;
      }
      if (QCollision[state][i] > max) {
        max = QCollision[state][i];
        bestActionMax = i;
      }
    }
    
    for (int i = 0; i < numOfStates; i++){
      if (equal == QCollision[memState][i]){
        equalAction = true;
      }
    }
    if (equalAction == true){
      return bestActionEqual;
    }
    else{
      return bestActionMax;
    }
  }
}

int directReward(int action, bool distance){
	if (distance == false){
		return R[nextState][action];
	}
	else {
		return RCollision[currentState][action];
	}
}

int maxNextQ(int action, bool distance){
	int currentMax = 0;
	if (distance == false){
		for (int i = 0; i < numOfStates; i++){
			if (Q[nextState][i] > currentMax)
			{
				currentMax = Q[nextState][i];
			}
		}
	}
	else{
		for (int i = 0; i < numOfStates; i++){
			if (QCollision[currentState][i] > currentMax)
			{
				currentMax = QCollision[currentState][i];
			}
		}
	}
	if (currentMax <= directReward(action, distance))
	{
		currentMax = 0;
	}
	return currentMax;
}

int calcQ(int action, bool distance){
	if (distance == false){
		return static_cast<int>(directReward(action, distance) + gammah*maxNextQ(action, distance));
	}
	else{
		return static_cast<int> (directReward(action, distance));
	}
}

int Q_get(int Q_index1, int Q_index2)
{return Q[Q_index1][Q_index2];}

//CHECK DISTANCE TO OBJECT
bool distanceCheck(int distance){
  bool avoid = false;
  
  if (distance < 200){
    avoid = true;
  }
  else {
    avoid = false;
  }
  return avoid;
}

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

