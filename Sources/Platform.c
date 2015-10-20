/*
 * Platform.c
 *
 *  Created on: 21.03.2015
 *      Author: dave_sch_6
 */


#include "Platform.h"
#include "Component_Drivers/LED.h"
#include "Component_Drivers/Keys.h"
#include "Component_Drivers/PIR.h"
#include "Component_Drivers/LDR.h"
#include "Event.h"
#include "Timer.h"
#include "RTOS.h"
#include "Shell.h"
#include "RSCom.h"
#include "Sem.h"
#include "NVM_Config.h"
#include "NodeConfig.h"
#include "SensData.h"
#include "Component_Drivers/Trigger.h"

void PL_Init(void)
{
#if PL_HAS_LED
	LED_Init();
#endif
	KEY_Init();
	PIR_Init();
	EVENT_Init();
	TMR_Init();
	RTOS_Init();
	SHELL_Init();
	RS_Init();
	SEM_Init();
	NVMC_Init();
	NC_Init();
	Sens_Init();
	LDR_Init();
	TRG_Init();
}

void PL_Deinit(void)
{
#if PL_HAS_LED
	LED_Deinit();
#endif
	KEY_Deinit();
	PIR_Deinit();
	EVENT_Deinit();
	TMR_Deinit();
	RTOS_Deinit();
	SHELL_Deinit();
	RS_Deinit();
	NVMC_Deinit();
}
