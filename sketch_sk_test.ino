// ScoreKeeper

#include <SimbleeForMobile.h>

//#include "SparkFun_ADXL345.h"
#include "controller.h"
#include "ui_handles.h"

#define MAIN_SCREEN 1

//const int ch1_cs_pin = 6;
//ADXL345 *acc[4];
//ADXL345 *ch1;

//AccController ac;

enum state_t {IDLE=0, ARMED, TRIGGERED};

state_t state;

uint8_t hText1;
uint8_t hText2;
uint8_t hText3;

bool uiIsReady = false;


void setup() {
	// put your setup code here, to run once:

	SimbleeForMobile.deviceName = "ScoreKeeper";
	SimbleeForMobile.txPowerLevel = 4;
	SimbleeForMobile.begin();

	Serial.begin(9600);

	Serial.println("Starting");
	Serial.println("Build: 12/3/16");
	
	ac.Init(1);
	ac.Reset();
	//ac.Start();
	state = IDLE;


}

char str[40];

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

int currentScreen = -1;

void ui()
{
  if (SimbleeForMobile.screen == currentScreen)
    return;
  
  currentScreen = SimbleeForMobile.screen;

  switch (currentScreen)
  {
    case MAIN_SCREEN:
      createMain1Screen();
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
  }

}


const char * const playerSegmentNames[4] = {"1", "2", "3", "4"};

void createMainScreen()
{
	SimbleeForMobile.beginScreen(WHITE, PORTRAIT);
	
	bMainTest = SimbleeForMobile.drawButton(30,10, 150, "Test", BLUE, BOX_TYPE);
	bMainGame = SimbleeForMobile.drawButton(30,40, 150, "Start Game", BLUE, BOX_TYPE);
	
	segMainPlayers = SimbleeForMobile.drawSegment(30, 80, 400, playerSegmentNames, 4);
	//updateValue(segMainPlayers, DEFAULT_PLAYERS);
	
	stepMainShots = SimbleeForMobile.drawStepper(30, 110, 100, MIN_SHOTS, MAX_SHOTS, BLACK);
	stepMainRounds = SimbleeForMobile.drawStepper(30, 140, 100, MIN_ROUNDS, MAX_ROUNDS, BLACK);
	//updateValue(stepMainShots, DEFAULT_SHOTS);
	//updateValue(stepMainRounds, DEFAULT_ROUNDS);

	sprintf(str, "%d", DEFAULT_SHOTS);
	txtMainShots = SimbleeForMobile.drawText(150, 110, str, RED);
	
	sprintf(str, "%d", DEFAULT_ROUNDS);
	txtMainRounds = SimbleeForMobile.drawText(150, 110, str, RED);
	
	// set callbacks
	SimbleeForMobile.setEvents(bMainTest, EVENT_RELEASE);
	SimbleeForMobile.setEvents(bMainGame, EVENT_RELEASE);
	SimbleeForMobile.setEvents(segMainPlayers, EVENT_RELEASE);
	SimbleeForMobile.setEvents(stepMainShots, EVENT_RELEASE);
	SimbleeForMobile.setEvents(stepMainRounds, EVENT_RELEASE);
	

	SimbleeForMobile.endScreen();
	
	uiIsReady = true;
}




uint8_t hButton;

int count = 0;

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
		
		state = IDLE;
	
	}
}




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

