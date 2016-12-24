// ScoreKeeper

#include <SimbleeForMobile.h>

//#include "SparkFun_ADXL345.h"
#include "controller.h"
#include "manager.h"
#include "ui_handles.h"

#define MAIN_SCREEN 1
#define GAME_SCREEN 2
#define TEST_SCREEN 3


#define DARK_YELLOW ((color_t)0xAAAA00)

//const int ch1_cs_pin = 6;
//ADXL345 *acc[4];
//ADXL345 *ch1;

//AccController ac;

GameManager gm;

enum state_t {AIDLE=0, ARMED, TRIGGERED};

state_t state;

uint8_t hText1;
uint8_t hText2;
uint8_t hText3;

bool uiIsReady = false;


void setup() 
{
	// put your setup code here, to run once:

	SimbleeForMobile.deviceName = "ScoreKeeper";
	SimbleeForMobile.txPowerLevel = 4;
	SimbleeForMobile.begin();

	Serial.begin(9600);

	Serial.println("Starting");
	Serial.println("Build: 12/19/16");
	
	
	gm.Init();


}

char str[40];

void loop() {
	// put your main code here, to run repeatedly:
	
	SimbleeForMobile.process();
	
	if (!uiIsReady)
		return;
	
	// manage game
	gm.Loop();
	
	// handle ui updates
	if (gm.state.uiUpdateReq)
	{
		updateGameScreen();
		gm.state.uiUpdateReq = false;
	}
	
	
	
	delay(15);
	
}

void updateGameScreen(void)
{
	/*
	Serial.print("txtGameHist = ");
	Serial.print(txtGameHits);
	Serial.print("\n");
	Serial.print("bGameNextPlayer = ");
	Serial.print(bGameNextPlayer);
	Serial.print("\n");
	*/

	// update elements
	sprintf(str, "Round: %d", gm.state.curRound+1);
	SimbleeForMobile.updateText(txtGameRound, str);
	sprintf(str, "Player: %d", gm.state.curPlayer+1);
	SimbleeForMobile.updateText(txtGamePlayer, str);
	sprintf(str, "Shots hit: %d", gm.state.nHitsThisPlayer);
	SimbleeForMobile.updateText(txtGameHits, str);
	sprintf(str, "Round score: %d", gm.state.curPoints);
	SimbleeForMobile.updateText(txtGameScore, str);
	
	for (int i = 0; i<gm.cfg.nPlayer; i++)
	{
		sprintf(str, "Player %d: %d", i+1, gm.state.totalPoints[i]);
		SimbleeForMobile.updateText(txtGamePlayerScore[i], str);
	}
	
	/*
	Serial.print("Update UI: shot = ");
	Serial.print(gm.shot);
	Serial.println("");
	*/
	// mode
	if (gm.mode == GAMEOVER)
	{
		SimbleeForMobile.updateText(txtGameStatus, "Game Over");
		SimbleeForMobile.updateColor(txtGameStatus, BLUE);
	}
	else if (gm.shot == WAIT)
	{
		SimbleeForMobile.updateText(txtGameStatus, "SHOOT");
		SimbleeForMobile.updateColor(txtGameStatus, GREEN);
	}
	else if (gm.shot == TIMEOUT)
	{
		SimbleeForMobile.updateText(txtGameStatus, "WAIT");
		SimbleeForMobile.updateColor(txtGameStatus, RED);
	}
	else if (gm.shot == PAUSED)
	{
		SimbleeForMobile.updateText(txtGameStatus, "PAUSED");
		SimbleeForMobile.updateColor(txtGameStatus, DARK_YELLOW);
	}

}


int currentScreen = -1;

void ui()
{
	if (SimbleeForMobile.screen == currentScreen)
		return;

	currentScreen = SimbleeForMobile.screen;

	switch (currentScreen)
	{
	case MAIN_SCREEN:
		createMainScreen();
		break;
	case GAME_SCREEN:
		createGameScreen();
		break;
	case TEST_SCREEN:
		createTestScreen();
		break;
  
	}
  
}

void ui_event(event_t &event)
{
	switch(currentScreen)
	{
	case MAIN_SCREEN:
		handleMainScreenEvents(event);
		break;
	case GAME_SCREEN:
		handleGameScreenEvents(event);
		break;
	case TEST_SCREEN:
		handleTestScreenEvents(event);
		break;
		
	}

}


int uiPlayer = DEFAULT_PLAYERS;
int uiShots = DEFAULT_SHOTS;
int uiRounds = DEFAULT_ROUNDS;


uint8_t bShot1;


const char * const playerSegmentNames[4] = {"1", "2", "3", "4"};

void createMainScreen()
{
	int ytop = 0;

	SimbleeForMobile.beginScreen(WHITE, PORTRAIT);
	
	ytop += 30;
	bMainTest = SimbleeForMobile.drawButton(30,ytop, 150, "Test", BLUE, BOX_TYPE);
	ytop += 60;
	bMainGame = SimbleeForMobile.drawButton(30,ytop, 150, "Start Game", BLUE, BOX_TYPE);
	
	ytop += 60;
	txtMainShots = SimbleeForMobile.drawText(30, ytop, "Players:", RED, 16);

	ytop += 40;
	segMainPlayers = SimbleeForMobile.drawSegment(30, ytop, 250, playerSegmentNames, 4, BLACK);
	SimbleeForMobile.updateValue(segMainPlayers, uiPlayer-1);
	
	ytop += 60;
	stepMainShots = SimbleeForMobile.drawStepper(30, ytop, 100, MIN_SHOTS, MAX_SHOTS, BLACK);
	sprintf(str, "%d shots", uiShots);
	txtMainShots = SimbleeForMobile.drawText(150, ytop, str, RED, 32);
	SimbleeForMobile.updateValue(stepMainShots, uiShots);

	ytop += 70;
	stepMainRounds = SimbleeForMobile.drawStepper(30, ytop, 100, MIN_ROUNDS, MAX_ROUNDS, BLACK);
	sprintf(str, "%d rounds", uiRounds);
	txtMainRounds = SimbleeForMobile.drawText(150, ytop, str, RED, 32);
	SimbleeForMobile.updateValue(stepMainRounds, uiRounds);
	
	
	// set callbacks
	// these don't seem to have the desired effect
	SimbleeForMobile.setEvents(bMainTest, EVENT_RELEASE);
	SimbleeForMobile.setEvents(bMainGame, EVENT_RELEASE);
	SimbleeForMobile.setEvents(segMainPlayers, EVENT_RELEASE);
	SimbleeForMobile.setEvents(stepMainShots, EVENT_RELEASE);
	SimbleeForMobile.setEvents(stepMainRounds, EVENT_RELEASE);

	SimbleeForMobile.endScreen();
	
	uiIsReady = true;
}


void createGameScreen()
{
	int ytop = 0;
	

	SimbleeForMobile.beginScreen(WHITE, PORTRAIT);
	
	
	ytop += 30;
	bGameNextPlayer = SimbleeForMobile.drawButton(30,ytop, 150, "Shots Complete", BLUE, BOX_TYPE);
	bGamePause = SimbleeForMobile.drawButton(230, ytop, 50, "Pause", DARK_YELLOW, BOX_TYPE);
	
	ytop += 60;
	sprintf(str, "Round: %d", gm.state.curRound);
	txtGameRound = SimbleeForMobile.drawText(30, ytop, str, BLACK, 20);
	
	ytop += 40;
	sprintf(str, "Player: %d", gm.state.curPlayer+1);
	txtGamePlayer = SimbleeForMobile.drawText(30, ytop, str, RED, 20);

	ytop += 40;
	sprintf(str, "Shots hit: %d", gm.state.nHitsThisPlayer);
	txtGameHits = SimbleeForMobile.drawText(30, ytop, str, BLACK, 20);
	
	ytop += 40;
	sprintf(str, "Round score: %d", gm.state.curPoints);
	txtGameScore = SimbleeForMobile.drawText(30, ytop, str, BLUE, 20);
	ytop += 20;
	
	Serial.print("init players: ");
	Serial.print(gm.cfg.nPlayer);
	Serial.print("\n");
	// If this is variable, the IDs change, but the reported event ids don't
	bool isPlaying;
	for (int i = 0; i<MAX_PLAYERS; i++)
	{
		ytop += 30;
		sprintf(str, "Player %d: %d", i+1, gm.state.totalPoints[i]);
		txtGamePlayerScore[i] = SimbleeForMobile.drawText(30, ytop, str, BLACK, 18);
		isPlaying = (i < gm.cfg.nPlayer);
		SimbleeForMobile.setVisible(txtGamePlayerScore[i], isPlaying);
	}
	
	ytop += 50;
	txtGameStatus = SimbleeForMobile.drawText(30, ytop, "Starting", RED, 32);
		
	ytop += 60;
	bGameEnd = SimbleeForMobile.drawButton(30,ytop, 150, "End Game", BLACK, BOX_TYPE);
	
	
	
	// set callbacks
	// these don't seem to have the desired effect
	SimbleeForMobile.setEvents(bGameNextPlayer, EVENT_RELEASE);
	SimbleeForMobile.setEvents(bGameEnd, EVENT_RELEASE);
	SimbleeForMobile.setEvents(bGamePause, EVENT_RELEASE);

	SimbleeForMobile.endScreen();
	
	uiIsReady = true;
}

void createTestScreen()
{
	int ytop = 0;

	SimbleeForMobile.beginScreen(WHITE, PORTRAIT);
	

	SimbleeForMobile.endScreen();
	
	uiIsReady = true;
}



void handleMainScreenEvents(event_t &event)
{
	Serial.print("event ");
	Serial.print(event.id);
	Serial.print(" ");
	Serial.print("type ");
	Serial.print(event.type);
	Serial.print("\n");
	
	if (event.id == bMainTest && event.type == EVENT_RELEASE)
	{
		Serial.print("Start test\n");
		SimbleeForMobile.showScreen(TEST_SCREEN);
		uiIsReady = false;
	}
	else if (event.id == bMainGame && event.type == EVENT_RELEASE)
	{
		Serial.print("Start game\n");
		
		// get config
		gameConfig cfg;
		cfg.nPlayer = uiPlayer;
		cfg.nShots = uiShots;
		cfg.nRounds = uiRounds;
		
		// make sure to init gm before changing screens		
		gm.EnterGame(cfg);
		
		SimbleeForMobile.showScreen(GAME_SCREEN);
		uiIsReady = false;
		
	}
	else if (event.id == segMainPlayers)// && event.type == EVENT_PRESS)
	{
		uiPlayer = event.value + 1;
		Serial.print("Players: ");
		Serial.print(uiPlayer);
		Serial.print("\n");
	}
	else if (event.id == stepMainShots)// && event.type == EVENT_PRESS)
	{
		uiShots = event.value;
		sprintf(str, "%d shots", uiShots);
		SimbleeForMobile.updateText(txtMainShots, str);
		
		Serial.print("Shots: ");
		Serial.print(uiShots);
		Serial.print("\n");

	}
	else if (event.id == stepMainRounds)// && event.type == EVENT_PRESS)
	{
		uiRounds = event.value;
		sprintf(str, "%d rounds", uiRounds);
		SimbleeForMobile.updateText(txtMainRounds, str);

		Serial.print("Rounds: ");
		Serial.print(uiRounds);
		Serial.print("\n");
	}


	
}

void handleGameScreenEvents(event_t &event)
{
	Serial.print("game event ");
	Serial.print(event.id);
	Serial.print(" ");
	Serial.print("type ");
	Serial.print(event.type);
	Serial.print("\n");
	Serial.print("bGameEnd = ");
	Serial.print(bGameEnd);
	Serial.print("\n");
	
	if (event.id == bGameNextPlayer && event.type == EVENT_RELEASE)
	{
		Serial.print("Finish player round\n");
		gm.EndPlayer();
		
	}
	else if (event.id == bGameEnd && event.type == EVENT_RELEASE)
	{
		Serial.print("Exit game\n");
		gm.ExitMode();
		SimbleeForMobile.showScreen(MAIN_SCREEN);
		uiIsReady = false;

	}
	else if (event.id == bGamePause && event.type == EVENT_RELEASE)
	{
		gm.TogglePause();
	}
	
	
}


void handleTestScreenEvents(event_t &event)
{
	
	if (event.id == bGameNextPlayer && event.type == EVENT_RELEASE)
	{
		//SimbleeForMobile.showScreen(TEST_SCREEN);
	}
	else if (event.id == bGameEnd && event.type == EVENT_RELEASE)
	{
		gm.ExitMode();
		SimbleeForMobile.showScreen(MAIN_SCREEN);
		uiIsReady = false;

	}
	
	
}



/*

void loop() {
	// put your main code here, to run repeatedly:
	
	SimbleeForMobile.process();
	
	if (!uiIsReady)
		return;
	
	if (state == IDLE)
	{
		ac.Start();
		SimbleeForMobile.updateText(hText3,"Ready");
	}
	
	
	if (ac.Ready())
	{
		delay(10);
		ac.ReadSamples();
		ac.Stop();
		
		if (ac.fifo[0].valid)
		{
			//printSample(0);
			ac.getLimits();
			//printLimits(0);
			state = TRIGGERED;
		}
		else
			state = IDLE;
		
		//ac.Reset();
	}
	
	
	
	if (state == TRIGGERED)
	{
		SimbleeForMobile.updateText(hText3,"Triggered");
		
		sprintf(str, "%4d %4d %4d", (int)ac.lim[0].xMin*10, (int)ac.lim[0].yMin*10, (int)ac.lim[0].zMin*10);
		SimbleeForMobile.updateText(hText1, str);
		sprintf(str, "%4d %4d %4d", (int)ac.lim[0].xMax*10, (int)ac.lim[0].yMax*10, (int)ac.lim[0].zMax*10);
		SimbleeForMobile.updateText(hText2, str);
	
	}
	
	
	delay(250);
	
}


void printLimits(int ch)
{
	Serial.print("x: ");
	Serial.print(ac.lim[ch].xMin);
	Serial.print(" ");
	Serial.print(ac.lim[ch].xMax);
	Serial.println("");
	Serial.print("y: ");
	Serial.print(ac.lim[ch].yMin);
	Serial.print(" ");
	Serial.print(ac.lim[ch].yMax);
	Serial.println("");
	Serial.print("z: ");
	Serial.print(ac.lim[ch].zMin);
	Serial.print(" ");
	Serial.print(ac.lim[ch].zMax);
	Serial.println("");
}

*/

/*
void printSample(int ch)
{
	int i;
	
	for (i=0; i<FIFO_SIZE; i++)
	{
		Serial.print(ac.fifo[ch].samp[i]);
		Serial.print(" ");
	}
	
	Serial.println("");
}
*/

/*
void createMain1Screen()
{
	SimbleeForMobile.beginScreen(WHITE, PORTRAIT);

	hText1 = SimbleeForMobile.drawText(10, 50, "min");
	hText2 = SimbleeForMobile.drawText(10, 80, "max");
	hText3 = SimbleeForMobile.drawText(10, 110, "Status");

	hButton = SimbleeForMobile.drawButton(10, 150, 100, "Rearm", GREEN, BOX_TYPE);


	SimbleeForMobile.setEvents(hButton, EVENT_RELEASE);

	SimbleeForMobile.endScreen();

	uiIsReady = true;
}



void handleMainScreenEvents(event_t &event)
{
  
	if (event.id == hButton && event.type == EVENT_RELEASE)
	{
		//count += 1;
		//sprintf(str, "cnt=%d", count);
		//SimbleeForMobile.updateText(hText,str);
		
		state = AIDLE;
	
	}
}



*/




	//pinMode(ss_pin, OUTPUT);
	//digitalWrite(SS,HIGH);
	//SPI.begin();
	//SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
	

	
	//ch1->printAllRegister();

	
		/*
	char a = 0x29;
	digitalWrite(SS,LOW);
	SPI.transfer(a);
	digitalWrite(SS,HIGH);
	*/

	
		/*
	ch1->readAccel(&ax, &ay, &az);
	sprintf(str, "%8d %8d %8d", ax, ay, az);
	Serial.println(str);
	*/
	
	//sprintf(str,"%f %f %f", fa[0], fa[1], fa[2]);
	//dtostrf(str,10,3,str);
	
	/*
	ch1->get_Gxyz(fa);
	Serial.print(fa[0]);
	Serial.print(" ");
	Serial.print(fa[1]);
	Serial.print(" ");
	Serial.print(fa[2]);
	Serial.print(" ");
	Serial.print("\n");
	*/

	/*
	
		int ax, ay, az;
	char str[30];
	double fa[3];
	
	
	
	byte interrupts = ch1->getInterruptSource();
	uint8_t entries;
	bool fifoTrig;
	
	if (ch1->triggered(interrupts, ADXL345_ACTIVITY))
	{
		Serial.println("Act trigger");
		fifoTrig = ch1->isFifoTriggered(entries);
		Serial.print("Trig = ");
		Serial.print((int)fifoTrig);
		Serial.print(", entries = ");
		Serial.print(entries);
		Serial.println("");
		
		for (int i=0; i<32; i++)
			printSample();
			
		ch1->ResetTrigger();
	}
	
*/

