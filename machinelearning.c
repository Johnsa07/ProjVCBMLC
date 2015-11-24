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
void goToState(int state);
//bool collisionCheck(int action);


void testAllActions(int state){
  //Redo this method to test actions on the car
  //And wait after each issued action
	for (int i = 0; i < numOfStates; i++){
		Q[state][i] = calcQ(i);
	}
}

void goToState(int state){

	if (actionsTested[state] == false)
	{
		testAllActions(state);
		actionsTested[state] = true;
	}
	currentState = correctAction(state);
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


void initialize(){
	srand (time(NULL));
	for (int i = 0; i < numOfStates; i++){
		for (int j = 0; j < numOfStates; j++){
			Q[i][j] = 0;
			//QCollision[i][j] = 0;
		}
	}
}
