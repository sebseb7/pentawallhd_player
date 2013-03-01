#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "libs/math.h"

static uint16_t a = 0;

static uint8_t tick(void) {

	setDelay(chan[0]);
	if(a==0)
	{
		setLedAll(255,0,0);
		a=1;
	}
	else if(a==1)
	{
		setLedAll(0,255,0);
		a=2;
	}
	else if(a==2)
	{
		setLedAll(0,0,255);
		a=0;
	}


	return 0;
}

static void init(void)
{
	a = 0;
	chan[0]=29;
	chan[1]=29;
	chan[2]=127;
	chan[3]=0;
	chan[4]=0;
	chan[5]=0;
	chan[6]=0;
	chan[7]=127;
}
static void deinit(void)
{
}



static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
	registerAnimation(init,tick,deinit, 29, 3000);
}




