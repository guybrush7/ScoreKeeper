#include <SimbleeForMobile.h>

//#include "SparkFun_ADXL345.h"
#include "controller.h"


#define MAIN_SCREEN 1

//const int ch1_cs_pin = 6;
//ADXL345 *acc[4];
//ADXL345 *ch1;

AccController ac;


void setup() {
	// put your setup code here, to run once:

	SimbleeForMobile.deviceName = "ScoreKeeper";
	SimbleeForMobile.begin();


	Serial.begin(9600);

	Serial.println("Starting");
	Serial.println("Build: 12/3/16");
	
	ac.Init(1);
	ac.Reset();
	ac.Start();


}

void loop() {
	// put your main code here, to run repeatedly:
	
	if (ac.Ready())
	{
		delay(10);
		ac.ReadSamples();
		
		if (ac.fifo[0].valid)
			printSample(0);
		
		ac.Reset();
	}
		
	
	delay(250);
	
	//SimbleeForMobile.process();
}


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

uint8_t hText;
uint8_t hButton;

int count = 0;

void createMainScreen()
{
  SimbleeForMobile.beginScreen(WHITE, PORTRAIT);

  hText = SimbleeForMobile.drawText(10, 50, "Text here");

  hButton = SimbleeForMobile.drawButton(10, 80, 100, "Do", GREEN, BOX_TYPE);


  SimbleeForMobile.setEvents(hButton, EVENT_RELEASE);

  SimbleeForMobile.endScreen();


}

void handleMainScreenEvents(event_t &event)
{
  char str[20];
  
  if (event.id == hButton && event.type == EVENT_RELEASE)
  {
    count += 1;
    sprintf(str, "cnt=%d", count);
    SimbleeForMobile.updateText(hText,str);
	
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

