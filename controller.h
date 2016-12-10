


#include "SparkFun_ADXL345.h"


#define FIFO_SIZE 	32
#define MAX_ACCEL	4


const uint8_t accelRange = 16;
const double gainCorrection = (double)accelRange/2.0;
// 16 counts / 1g
const uint8_t activityThresh = 48;
const uint8_t accelMeasRate = ADXL345_BW_1600;

const int CS_PINS[4] = {6, 10, 11, 12};

struct fifobuf
{
	bool valid;
	float x[FIFO_SIZE];
	float y[FIFO_SIZE];
	float z[FIFO_SIZE];
};

struct accLimits
{
	float xMin;
	float xMax;
	float yMin;
	float yMax;
	float zMin;
	float zMax;
};


class AccController
{
private:
	ADXL345 *acc[MAX_ACCEL];
	int nAcc;
	
public:

	fifobuf fifo[MAX_ACCEL];
	accLimits lim[MAX_ACCEL];
	
	AccController();
	
	void Init(int nSen);
	void Reset(void);
	void Start(void);
	void Stop(void);

	bool Ready(void);
	
	void ReadSamples(void);
	
	void getLimits(void);

	

};


AccController::AccController()
{
}

// set up all accels, don't start
void AccController::Init(int nSen)
{
	int i;
	ADXL345 *ch;
	double ch_gain[3];
	
	nAcc = nSen;
	
	for (i=0; i<nAcc; i++)
	{
		acc[i] = new ADXL345(CS_PINS[i]);
		ch = acc[i];
		
		// Clear interrupts, power on
		ch->disableAllInterrupts();
		ch->powerOn();
		
		// Set range
		ch->setRangeSetting(accelRange);
		// No offset
		ch->setAxisOffset(0,0,0);
		// Set activity threshold
		ch->setActivityThreshold(activityThresh);
		
		// Correct gain from default for 2g limit
		ch->getAxisGains(ch_gain);
		ch_gain[0] *= gainCorrection;
		ch_gain[1] *= gainCorrection;
		ch_gain[2] *= gainCorrection;
		ch->setAxisGains(ch_gain);
		
		// set bandwidth/rate
		ch->set_bw(accelMeasRate);
		
		// Enable activity detection on all channels
		ch->setActivityX(true);
		ch->setActivityY(true);
		ch->setActivityZ(true);
		
		// Map to INT1
		ch->setInterruptMapping(ADXL345_INT_ACTIVITY_BIT, ADXL345_INT1_PIN);
		
		
		//ch->EnableTriggerMode(1,15);
		//ch->ActivityINT(1);
	}
}

// reset activity triger
void AccController::Reset(void)
{
	int i;
	
	for (i=0; i<nAcc; i++)
	{
		// invalidate samples
		fifo[i].valid = false;
		// rearm fifo trigger
		acc[i]->ResetTrigger();
	}
}

// enable triggers
void AccController::Start(void)
{
	int i;
	
	for (i=0; i<nAcc; i++)
	{
		acc[i]->EnableTriggerMode(1,15);
		acc[i]->ActivityINT(1);
	}
}

void AccController::Stop(void)
{
	int i;
	
	for (i=0; i<nAcc; i++)
	{
		acc[i]->DisableTriggerMode();
		acc[i]->ActivityINT(0);
	}
}

// are any triggers ready?
bool AccController::Ready(void)
{
	bool res = false;
	byte interrupts;
	int i;
	
	for (i=0; i<nAcc; i++)
		interrupts = acc[i]->getInterruptSource();
		if (acc[i]->triggered(interrupts, ADXL345_ACTIVITY))
			res = true;
	
	return res;
}

void AccController::ReadSamples(void)
{
	bool triggered;
	uint8_t nSamples;
	uint8_t i,j;
	double xyz[3];
	double Gmag; 
	
	for (i=0; i<nAcc; i++)
	{
		// Get FIFO status
		triggered = acc[i]->isFifoTriggered(nSamples);
		
		fifo[i].valid = triggered;
		
		Serial.println("Act trigger");
		Serial.print("Trig = ");
		Serial.print((int)triggered);
		Serial.print(", entries = ");
		Serial.print(nSamples);
		Serial.println("");		
		
		
		if (triggered)
		{
			// this accel was triggered, read the samples
			for (j=0; j<FIFO_SIZE; j++)
			{
				// read set of xyz samples, converted to g
				acc[i]->get_Gxyz(xyz);
				
				if (0)
				{
					Serial.print(xyz[0]);
					Serial.print(" ");
					Serial.print(xyz[1]);
					Serial.print(" ");
					Serial.print(xyz[2]);
					Serial.print(" ");
					Serial.print("\n");
				}
				
				// calculate the magnitude
				//Gmag = xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2];
				//Gmag = sqrt(Gmag);
				
				fifo[i].x[j] = (float)xyz[0];
				fifo[i].y[j] = (float)xyz[1];
				fifo[i].z[j] = (float)xyz[2];
				
				//fifo[i].samp[j] = (float)Gmag;
			}
		}
	}
}

void AccController::getLimits(void)
{
	int i,j;
	float x,y,z;
	
	for (i=0; i<nAcc; i++)
	{
		lim[i].xMin = 0;
		lim[i].xMax = 0;
		lim[i].yMin = 0;
		lim[i].yMax = 0;
		lim[i].zMin = 0;
		lim[i].zMax = 0;
	
		if (fifo[i].valid)
		{
			// find min/max for each channel
			for (j=0; j<FIFO_SIZE; j++)
			{
				x = fifo[i].x[j];
				y = fifo[i].y[j];
				z = fifo[i].z[j];
			
				if (x < lim[i].xMin)
					lim[i].xMin = x;
				if (x > lim[i].xMax)
					lim[i].xMax = x;
				if (y < lim[i].yMin)
					lim[i].yMin = y;
				if (y > lim[i].yMax)
					lim[i].yMax = y;
				if (z < lim[i].zMin)
					lim[i].zMin = z;
				if (z > lim[i].zMax)
					lim[i].zMax = z;
			}
		}
	}
}












































