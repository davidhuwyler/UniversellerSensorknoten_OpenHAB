/*
 * Event.h
 *
 *  Created on: 21.03.2015
 *      Author: dave_sch_6
 */

/*!
** @file Event.h
** @version 01.00
** @author David Schwegler, david.schwegler@stud.hslu.ch
** @brief
**      This module implements a generic event driver. We are using numbered events
**      EVENT_HadleEvent() can be used to process the pending events. The Event witch the lowest
**      number has the highest priority
** @detail
**
*/


#ifndef SOURCES_EVENT_H_
#define SOURCES_EVENT_H_

#include "PE_Types.h"
#include "Platform.h"

typedef enum EVENT_Handle{
	EVENT_STARTUP, 		/*!< System startup Event */
	EVENT_HEARTBEAT,	/*!< blinking Heartbeat of the Board*/
#if PL_NOF_PIR >= 1
	EVENT_PIR1_MOTION,	/*!< motion detected*/
#endif
#if PL_NOF_PIR >= 2
	EVENT_PIR2_MOTION,	/*!< motion detected*/
#endif
#if PL_NOF_KEYS >= 1
	EVENT_SW1_PRESSED,
	EVENT_SW1_LPRESSED,
	EVENT_SW1_RELEASED,
#endif
#if PL_NOF_KEYS >= 2
	EVENT_SW2_PRESSED,
	EVENT_SW2_LPRESSED,
	EVENT_SW2_RELEASED,
#endif
	EVENT_NOF_EVENTS	/*!< Must be last one! */
}EVENT_Handle;


/*!
 *	\brief Sets an event.
 *	\param[in] event The handl eof event to start.
 */
void EVENT_SetEvent(EVENT_Handle event);

/*!
 *	\brief Clears an Event
 *	\param[in] event The handle of event to remove.
 */
void EVENT_ClearEvent(EVENT_Handle event);

/*!
 *	\brief Returns the status of an Event
 *	\param[in] event The handle of event to check
 *	\return TRUE if the Event is set, FALSE if not.
 */
bool EVENT_EventIsSet(EVENT_Handle event);

/*!
 *	\brief Returns the status of an Event. As side Effect the event is cleared.
 *	\param[in] event The handle of event to check.
 *	\return TRUE if the Event is set, FALSE if not.
 */
bool EVENT_EventIsSetAutoClear(EVENT_Handle event);

/*!
 *	\brief Routine to check if an event is pending. If an event is pending, the event is cleared and the provided callback is called.
 *	\param[in] callback Callback routine to be called. The event handle is passed as argument to the callback.
 */
void EVENT_HandleEvent(void (*callback)(EVENT_Handle));

/* \brief Event module initialization */
void EVENT_Init(void);

/* \brief Event module deinitialization */
void EVENT_Deinit(void);


#endif /* SOURCES_EVENT_H_ */
