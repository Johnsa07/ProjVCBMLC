
#define bool int
#define true 1
#define false 0


void initialize();
int calcQ(int action);
int directReward(int action);
int maxNextQ(int state);
void testAllActions(int state);
int correctAction(int state);
int goToState(int state);
bool collisionCheck(int action);