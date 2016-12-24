
#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "SparkFun_ADXL345.h"


#define FIFO_SIZE 	32
#define MAX_ACCEL	4



struct fifobuf
{
	bool valid;
	float x[FIFO_SIZE];
	float y[FIFO_SIZE];
	float z[FIFO_SIZE];
};

struct accLimits
{
	bool valid;
	float xMin;
	float xMax;
	float yMin;
	float yMax;
	float zMin;
	float zMax;
	float absMax;
	int maxCh;
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
	
	void SPI_Init(void);
	void Init(int nSen);
	void Reset(void);
	void Start(void);
	void Stop(void);

	bool Ready(void);
	
	bool ReadSamples(void);
	
	void getLimits(void);
	
	float chMaxAccel;
	int chMaxIdx;

	

};



#endif










































