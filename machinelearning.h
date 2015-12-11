
#define bool int
#define true 1
#define false 0


void initialize(); //Klar
int calcQ(int action, bool distance);
int directReward(int action, bool distance);
int maxNextQ(int state, bool distance);
void testAllActions(int state, bool distance);
int correctAction(int state, bool distance);
int goToState(int state);
bool validTest(int state);
bool collisionCheck(int action);
bool distanceCheck(int action);
