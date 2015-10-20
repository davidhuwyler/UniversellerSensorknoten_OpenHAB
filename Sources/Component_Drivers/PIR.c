/*
 * PIR.c
 *
 *  Created on: 07.04.2015
 *      Author: David Schwegler
 */

#include "Platform.h"
#include "PIR.h"
#include "Event.h"
#include "KeyDebounce.h"
#include "Trigger.h"


void PIR_Scan(void) {
	/* used if there is a polled Sensor*/
}

static void PIR1_Trigger(void *p){
	(void)p;
	if(PIR1_Get()){
		EVENT_SetEvent(EVENT_PIR1_MOTION);
	}
}

static void PIR2_Trigger(void *p){
	(void)p;
	if(PIR2_Get()){
		EVENT_SetEvent(EVENT_PIR2_MOTION);
	};
}

void  PIR_OnInterrupt(KEY_PIR PIR_sens){

  switch(PIR_sens) {
#if PL_NOF_PIR >= 1
    case KEY_PIR1:
    	TRG_SetTrigger(TRG_MOTION_FILTER,200/TRG_TICKS_MS,PIR1_Trigger,NULL);
    	//EVENT_SetEvent(EVENT_PIR1_MOTION);
    	break;
#endif
#if PL_NOF_PIR >= 2
    case KEY_PIR2:
    	TRG_SetTrigger(TRG_MOTION_FILTER,200/TRG_TICKS_MS,PIR2_Trigger,NULL);
    	//EVENT_SetEvent(EVENT_PIR2_MOTION);
    	break;
#endif
    default:
      /* unknown? */
      break;
  } /* switch */
}


/*! \brief PIR driver initialization */
void PIR_Init(void) {
  /* nothing needed for now */
}

/*! \brief PIR driver de-initialization */
void PIR_Deinit(void) {
  /* nothing needed for now */
}
