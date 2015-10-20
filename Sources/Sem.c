/*
 * Sem.c
 *
 *  Created on: 18.04.2015
 *      Author: David Schwegler
 */


#include "Platform.h" /* interface to the platform */
#include "FRTOS1.h"
#include "Sem.h"
#include "WAIT1.h"
#include "AS1.h"
#include "RSCom.h"

#define SEM_MAX_CNT 2


static SemaphoreHandle_t MySem = NULL;

void SEM_give_RSsemFromISR(void){
	long notNeeded;
	if(FRTOS1_xSemaphoreGiveFromISR(MySem,&notNeeded)){

	}
}

bool SEM_takeRS_sem(void){
	return FRTOS1_xSemaphoreTake(MySem,portMAX_DELAY);
}


/*! \brief Initializes module */
void SEM_Init(void) {
  SemaphoreHandle_t xSemaphore = NULL;
  MySem = xSemaphoreCreateCounting(AS1_INP_BUF_SIZE/NOF_BYTES_IN_TICKET,0);
  //vSemaphoreCreateBinary(MySem);
  if (MySem == NULL) {
    for(;;); /* creation failed */
  }
  FRTOS1_vQueueAddToRegistry(MySem,"Semaphore")
}


void SEM_Deinit(void) {
  /* nothing */
}
