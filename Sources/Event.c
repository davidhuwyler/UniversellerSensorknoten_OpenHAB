/*
 * Event.c
 *
 *  Created on: 21.03.2015
 *      Author: dave_sch_6
 */

/*!
** @file Event.c
** @version 01.00
** @author David Schwegler, david.schwegler@stud.hslu.ch
** @brief
**      This module implements a generic event driver. We are using numbered events
**      EVENT_HadleEvent() can be used to process the pending events. The Event witch the lowest
**      number has the highest priority
** @detail
**
*/

#include "Event.h"
#include "CS1.h"

typedef uint8_t EVENT_MemUnit; /*!< memory unit used to store events flags */
#define EVENT_MEM_UNIT_NOF_BITS  (sizeof(EVENT_MemUnit)*8) /*!< number of bits in memory unit */

static EVENT_MemUnit EVENT_Events[((EVENT_NOF_EVENTS-1)/EVENT_MEM_UNIT_NOF_BITS)+1]; /*!< Bit set of events */

#define SET_EVENT(event) \
  EVENT_Events[(event)/EVENT_MEM_UNIT_NOF_BITS] |= (1<<(EVENT_MEM_UNIT_NOF_BITS-1))>>((uint8_t)((event)%EVENT_MEM_UNIT_NOF_BITS)) /*!< Set the event */
#define CLR_EVENT(event) \
  EVENT_Events[(event)/EVENT_MEM_UNIT_NOF_BITS] &= ~((1<<(EVENT_MEM_UNIT_NOF_BITS-1))>>((uint8_t)((event)%EVENT_MEM_UNIT_NOF_BITS))) /*!< Clear the event */
#define GET_EVENT(event) \
  (bool)(EVENT_Events[(event)/EVENT_MEM_UNIT_NOF_BITS]&((1<<(EVENT_MEM_UNIT_NOF_BITS-1))>>((uint8_t)((event)%EVENT_MEM_UNIT_NOF_BITS)))) /*!< Return TRUE if event is set */

void EVENT_SetEvent(EVENT_Handle event){
	CS1_CriticalVariable();

	CS1_EnterCritical();
	SET_EVENT(event);
	CS1_ExitCritical();
}

void EVENT_ClearEvent(EVENT_Handle event){
	CS1_CriticalVariable();

	CS1_EnterCritical();
	CLR_EVENT(event);
	CS1_ExitCritical();
}

bool EVENT_EventIsSet(EVENT_Handle event) {
  return GET_EVENT(event);
}

bool EVENT_EventIsSetAutoClear(EVENT_Handle event) {
  bool res;

  res = GET_EVENT(event);
  if (res) {
    CLR_EVENT(event); /* automatically clear event */
  }
  return res;
}

void EVENT_HandleEvent(void (*callback)(EVENT_Handle)) {
  /* Handle the one with the highest priority. Zero is the event with the highest priority. */
   EVENT_Handle event;
   CS1_CriticalVariable();

   CS1_EnterCritical();
   for (event=(EVENT_Handle)0; event<EVENT_NOF_EVENTS; event++) { /* does a test on every event */
     if (GET_EVENT(event)) { /* event present? */
       CLR_EVENT(event); /* clear event */
       break; /* get out of loop */
     }
   }
   CS1_ExitCritical();
   if (event != EVENT_NOF_EVENTS) {
     callback(event);
     /* Note: if the callback sets the event, we will get out of the loop.
      * We will catch it by the next iteration.
      */
   }
}

void EVENT_Init(void) {
  uint8_t i;

  i = 0;
  do {
    EVENT_Events[i] = 0; /* initialize data structure */
    i++;
  } while(i<sizeof(EVENT_Events)/sizeof(EVENT_Events[0]));
}

void EVENT_Deinit(void) {
  /* nothing needed */
}



