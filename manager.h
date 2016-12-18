

#include "controller.h"


enum gameMode {IDLE = 0; TEST, GAME, GAMEOVER};

enum shotState {IDLE = 0; WAIT; TIMEOUT; PAUSE};

#define MAX_PLAYERS		4
#define MAX_ROUNDS		10
#define MAX_SHOTS		10
#define NUM_TARGETS		4

#define MIN_ROUNDS		1
#define MIN_SHOTS		1

#define DEFAULT_PLAYERS 2
#define DEFAULT_ROUNDS	5
#define DEFAULT_SHOTS	10

int targetValue[NUM_TARGETS] = {1, 2, 3, 2};
int missValue = -1;


struct gameState
{
	bool uiUpdateReq;
	int curPlayer;
	int nHitsThisPlayer;
	int curPoints;
	int totalPoints[MAX_PLAYERS];
	int curRound;
	
};

struct gameConfig
{
	int nPlayer;
	int nShots;
	int nRounds;
};

class GameManager
{
public:
	GameManager();
	void Init(void);
	
	// User input
	void EnterGame(gameConfig cfg);
	void ExitMode();
	void EnterTest();
	void EndPlayer();
	
	void Loop();
	
	
	gameState state;
	shotState shot;
	
private:
	
	gameConfig gamecfg;
	gameMode mode;
	
	AccController ac;
	
	void InitRound(void);
	void InitPlayer(void);
	
	void FinishPlayer(void);
	void FinishRound(void);
	
	bool CheckForShot(void);
	
	bool accelActive;


};



