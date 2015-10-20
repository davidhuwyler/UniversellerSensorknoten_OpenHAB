/*
 * LED.h
 *
 *  Created on: 24.03.2015
 *      Author: dave_sch_6
 */

#ifndef SOURCES_LED_H_
#define SOURCES_LED_H_

#include "LED_1.h"
#include "LED_2.h"
#include "LED_3.h"
#include "CLS1.h"


#define LED1_On()     LED_1_On()
#define LED1_Off()    LED_1_Off()
#define LED1_Neg()    LED_1_Neg()
#define LED1_Get()    LED_1_Get()
#define LED1_Put(val) LED_1_Put(!val)
#define LED1_Init()   /* do nothing */
#define LED1_Deinit() /* do nothing */

#define LED2_On()     LED_2_On()
#define LED2_Off()    LED_2_Off()
#define LED2_Neg()    LED_2_Neg()
#define LED2_Get()    LED_2_Get()
#define LED2_Put(val) LED_2_Put(!val)
#define LED2_Init()   /* do nothing */
#define LED2_Deinit() /* do nothing */

#define LED3_On()     LED_3_On()
#define LED3_Off()    LED_3_Off()
#define LED3_Neg()    LED_3_Neg()
#define LED3_Get()    LED_3_Get()
#define LED3_Put(val) LED_3_Put(!val)
#define LED3_Init()   /* do nothing */
#define LED3_Deinit() /* do nothing */


void LED_Init(void);
void LED_Deinit(void);

byte LED_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);


#endif /* SOURCES_LED_H_ */
