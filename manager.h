
#ifndef _MANAGER_H
#define _MANAGER_H


#include "controller.h"


enum gameMode {IDLE = 0, TEST, GAME, GAMEOVER};

enum shotState {DISABLED = 0, WAIT, TIMEOUT, PAUSED, TEST_TRIG};

#define MAX_PLAYERS		4
#define MAX_ROUNDS		10
#define MAX_SHOTS		10
#define NUM_TARGETS		4

#define MIN_ROUNDS		1
#define MIN_SHOTS		1

#define DEFAULT_PLAYERS 2
#define DEFAULT_ROUNDS	5
#define DEFAULT_SHOTS	10

#define TIMEOUT_DURATION_MS 	3000

#define NUM_ACCEL 		4



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
	void EnterGame(gameConfig initcfg);
	void EnterTest(void);
	void ExitMode();
	void EndPlayer();
	
	// update ui
	gameState GetState(void);
	
	void ArmTest(void);
	
	void Loop();
	
	void TogglePause(void);
	
	
	gameState state;
	shotState shot;
	gameConfig cfg;
	gameMode mode;
	AccController ac;
	
private:
	
	uint32_t timeoutStartTime;
	
	void InitRound(void);
	void InitPlayer(void);
	
	void FinishPlayer(void);
	void FinishRound(void);
	
	bool CheckForShot(void);
	
	void ScoreHit(void);
	
	bool accelActive;


};

#endif

