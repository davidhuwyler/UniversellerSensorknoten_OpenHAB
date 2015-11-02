/*
 * Application.c
 *
 *  Created on: 21.03.2015
 *      Author: dave_sch_6
 */

/*!
** @file Application.c
** @version 01.00
** @author David Schwegler, david.schwegler@stud.hslu.ch
** @brief
**  	Application main routine implementation.
**      This module is our main loop program so you don't have to modify main.c.
**      This module manage all events.
** @detail
**
*/

#include "Platform.h"
#include "Application.h"
#include "Event.h"
#include "Component_Drivers/LED.h"
#include "WAIT1.h"
#include "RTOS.h"
#include "SensData.h"


/*!
 * \brief Application event handler
 * \param event Event to be handled
 */
static void APP_HandleEvents(EVENT_Handle event)
{
	switch(event){
	case EVENT_STARTUP:
#if PL_HAS_LED
		LED1_On();
		WAIT1_Waitms(50);
		LED1_Off();
#endif
		break;
	case EVENT_HEARTBEAT:
#if PL_HAS_LED
		LED1_Neg();
#endif
		break;
	case EVENT_PIR1_MOTION:
		Sens_eventDetected(SENS_PIR1_EVENT);
		CLS1_SendStr("PIR1 motion detected\r\n", CLS1_GetStdio()->stdOut);
		break;
	case EVENT_PIR2_MOTION:
		Sens_eventDetected(SENS_PIR2_EVENT);
		CLS1_SendStr("PIR2 motion detected\r\n", CLS1_GetStdio()->stdOut);
		break;
#if PL_NOF_KEYS >= 1
    case EVENT_SW1_PRESSED:
    	CLS1_SendStr("SW1\r\n", CLS1_GetStdio()->stdOut);
    	break;
    case EVENT_SW1_LPRESSED:
    	CLS1_SendStr("SW1 long\r\n", CLS1_GetStdio()->stdOut);
    	break;
    case EVENT_SW1_RELEASED:
    	CLS1_SendStr("SW1 release\r\n", CLS1_GetStdio()->stdOut);
    	break;
    	break;
#endif
#if PL_NOF_KEYS >= 2
    case EVENT_SW2_PRESSED:
    	CLS1_SendStr("SW2 short\r\n", CLS1_GetStdio()->stdOut);
    	break;
    case EVENT_SW2_LPRESSED:
    	CLS1_SendStr("SW2 long\r\n", CLS1_GetStdio()->stdOut);
    	break;
    case EVENT_SW2_RELEASED:
    	CLS1_SendStr("SW2 release\r\n", CLS1_GetStdio()->stdOut);
    	break;
#endif
	default:
		break;
	}
}

/*!
 * \brief Application main 'task'.
 */
static void APP_Task(void)
{
	EVENT_SetEvent(EVENT_STARTUP);	/* set startup event */
	for(;;){
		EVENT_HandleEvent(APP_HandleEvents);
		FRTOS1_vTaskDelay(20/portTICK_RATE_MS);
	}
}

void runTask(){
	APP_Task();
}

/*!
 * \brief Application initialization
 */
void initApplication(){
	PL_Init();
}

/*!
 * \brief Application main loop
 *
 */
void runApplication(){
	RTOS_Run();
	//APP_Task();
}

/*!
 * \brief Application deinitialization. Note that this code normally never is executed!
 *
 */
void deinitApplication(){
	PL_Deinit();
	/*! do nothing*/
}







