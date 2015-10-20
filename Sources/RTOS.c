/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#include "RTOS.h"
#include "FRTOS1.h"
#include "Component_Drivers/LED.h"
#include "Application.h"
#include "CDC1.h"
#include "USB1.h"
#include "WAIT1.h"

static uint8_t cdc_buffer[USB1_DATA_BUFF_SIZE];
static uint8_t in_buffer[USB1_DATA_BUFF_SIZE];

/*! \brief Task which handles the events
 *
 *	detailed description is the following...
 */
static void Task_HandleEvents (void *pvParams)
{
	(void)pvParams;
	for(;;){
		runTask();
	}

}


void RTOS_Run(void) {
  FRTOS1_vTaskStartScheduler();
  /* does usually not return here */
}

void RTOS_Init(void) {

	if (FRTOS1_xTaskCreate(Task_HandleEvents, (signed portCHAR *)"Handle_Tasks", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
		for(;;){}
	}

}

void RTOS_Deinit(void) {
}

