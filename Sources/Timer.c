/*
 * Timer.c
 *
 *  Created on: 24.03.2015
 *      Author: dave_sch_6
 */


#include "Timer.h"
#include "Event.h"
#include "Component_Drivers/Trigger.h"

void TMR_OnInterrupt(void)
{

	TRG_IncTick();
	/* this method gets called from TimerInt(TI1) */
	static int cntr=0;
	cntr++;

	if(cntr==(1000/TMR_TICK_MS))
	{
		EVENT_SetEvent(EVENT_HEARTBEAT);
		cntr = 0;
	}

}

void TMR_Init(void)
{
	/* nothing needed right now */
}


void TMR_Deinit(void)
{
	/* nothing needed right now */
}

