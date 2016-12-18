



GameManager::GameManager()
{
	mode = IDLE;
	accelActive = false;
	
}

void GameManager::Init(void)
{
	ac.Init(1);
	ac.Reset();

	
	
	
}

void GameManager::EnterGame(gameConfig cfg)
{
	gamecfg = cfg;
	mode = GAME;
	
	// init game state
	state.uiUpdateReq = true;
	state.curRound = 0;
	
	for (int i=0; i<cfg.nPlayer; i++)
		state.totalPoints[i] = 0;
	
	InitRound();
		
	shot = WAIT;

}

void GameManager::EnterTest(void)
{
	mode = TEST;
}

void GameManager::ExitMode(void)
{
	mode = IDLE;
}


void GameManager::InitRound(void)
{
	curPlayer = 0;
	
	InitPlayer();
}

void GameManager::InitPlayer(void)
{
	nHitsThisPlayer = 0;
	curPoints = 0;
}

void GameManager::FinishPlayer(void)
{
	int nMiss;
	
	// Calculate number of missed shots
	nMiss = cfg.nShots - state.nHitsThisPlayer;
	
	// Apply miss score
	if (nMiss > 0)
		state.curPoints += (nMiss * missValue);

	// Add current score to total
	state.totalPoints[curPlayer] += state.curPoints;

	// next player
	curPlayer += 1;
	
	if (curPlayer >= cfg.nPlayer)
	{
		// round over
		FinishRound();
	}
	else
	{
		// same round, next player
		InitPlayer();
	}
}

void GameManager::FinishRound(void)
{
	// next round
	curRound += 1;
	
	// Is the game over?
	if (curRound >= cfg.nRounds)
	{
		mode = GAMEOVER;
	}
	else
	{
		// start over at player 0
		curPlayer = 0;
		InitRound();
	}
	
}


// Called when a user touches the end round (player) button
void GameManager::EndPlayer()
{
	FinishPlayer();
}


// 
bool GameManager::CheckForShot(void)
{
	
	
	
	
}


void GameManager::Loop(void)
{
	
	// 
	
	
	
	
	
}






