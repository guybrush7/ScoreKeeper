

#include "manager.h"


int targetValue[NUM_TARGETS] = {1, 2, 3, 2};
int missValue = -1;



GameManager::GameManager()
{
	mode = IDLE;
	shot = DISABLED;
	accelActive = false;
	
}

void GameManager::Init(void)
{
	ac.SPI_Init();


	ac.Init(4);
	ac.Reset();
	
	
	
	
}

gameState GameManager::GetState(void)
{
	return state;
}

void GameManager::EnterGame(gameConfig initcfg)
{
	cfg = initcfg;
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
	shot = DISABLED;
}


void GameManager::InitRound(void)
{
	state.curPlayer = 0;
	
	InitPlayer();
}

void GameManager::InitPlayer(void)
{
	state.nHitsThisPlayer = 0;
	state.curPoints = 0;
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
	state.totalPoints[state.curPlayer] += state.curPoints;

	// next player
	state.curPlayer += 1;
	
	if (state.curPlayer >= cfg.nPlayer)
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
	state.curRound += 1;
	
	// Is the game over?
	if (state.curRound >= cfg.nRounds)
	{
		// hold player/round at final value
		state.curRound = cfg.nRounds-1;
		state.curPlayer = cfg.nPlayer-1;
		
		// set mode
		mode = GAMEOVER;
		
		// stop monitoring shots
		shot = DISABLED;
	}
	else
	{
		// start over at player 0
		state.curPlayer = 0;
		InitRound();
	}
	
}


// Called when a user touches the end round (player) button
void GameManager::EndPlayer()
{
	if (mode == GAMEOVER)
		return;
		
	FinishPlayer();
	state.uiUpdateReq = true;
}


// 
bool GameManager::CheckForShot(void)
{
	// Check if any interrupt has occured
	if (ac.Ready())
	{
		// wait to finish collecting
		delay(10);
		// Read the samples out of the FIFO
		if (ac.ReadSamples())
		{
			// Stop collecting
			// May have triggered again by now?
			ac.Stop();
			accelActive = false;
		
			// Get all limits
			ac.getLimits();
			
			// clear out
			ac.Reset();
			//ac.Stop();
		
			return true;
		}
	}
		
	return false;
	
}


void GameManager::Loop(void)
{
	// If we are Idle, make sure we are stopped
	if (shot == DISABLED)
		if (accelActive)
		{
			ac.Stop();
			accelActive = false;
		}
	
	// Waiting for shot
	if (shot == WAIT)
	{
		// If we aren't listening, start
		if (!accelActive)
		{
			// clear to be safe
			ac.Stop();
			// Start up
			ac.Start();
			
			accelActive = true;
		}
		else
		{
			// already listening, check on hit
			if (CheckForShot())
			{
				// got a hit
				if (mode == GAME)
				{
					ScoreHit();
					state.uiUpdateReq = true;
				}
				
				// Mark time
				timeoutStartTime = millis();
				// Start timeout
				shot = TIMEOUT;
				
				// Stop accel
				//ac.Stop();
				//accelActive = false;
				
				// show it
				state.uiUpdateReq = true;
			}
		}
	}
	
	// Wait for timer to complete
	if (shot == TIMEOUT)
	{
		uint32_t currentTime = millis();
		uint32_t time_diff = currentTime - timeoutStartTime;
		
		// check for expired timer
		if (time_diff > TIMEOUT_DURATION_MS)
		{
			shot = WAIT;
			state.uiUpdateReq = true;
		}
	}
	
	// If paused, make sure to stop accel
	if (shot == PAUSED)
	{
		if (accelActive)
		{
			ac.Stop();
			accelActive = false;
		}
	}
	
	
	
}


void GameManager::ScoreHit(void)
{
	int hitScore = 0;
	
	if (ac.chMaxIdx >= 0)
	{
		// score shot
		hitScore = targetValue[ac.chMaxIdx];
		
		// add to total
		state.curPoints += hitScore;
		state.nHitsThisPlayer += 1;
	}
	
}


void GameManager::TogglePause(void)
{
	if (mode == GAMEOVER)
		return;
	
	if (shot == PAUSED)
	{
		Serial.println("Pause off");
		shot = WAIT;
		state.uiUpdateReq = true;
	}
	else if (shot == WAIT)
	{
		Serial.println("Pause on");
		shot = PAUSED;
		state.uiUpdateReq = true;
	}
}



