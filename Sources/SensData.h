/*
 * SensData.h
 *
 *  Created on: 26.04.2015
 *      Author: David Schwegler
 */

#ifndef SENSDATA_H_
#define SENSDATA_H_

#include "Platform.h"
#include "CLS1.h"

#define	SENS_LIGHT1_EVENT 	(1<<1)
#define	SENS_LIGHT2_EVENT 	(1<<2)
#define	SENS_TEMP1_EVENT	(1<<3)
#define	SENS_TEMP2_EVENT	(1<<4)
#define	SENS_RH1_EVENT	 	(1<<5)
#define	SENS_RH2_EVENT	 	(1<<6)
#define	SENS_REED1_EVENT	(1<<7)
#define	SENS_REED2_EVENT	(1<<8)
#define	SENS_REED3_EVENT	(1<<9)		//not avaliable
#define	SENS_PIR1_EVENT		(1<<10)
#define	SENS_PIR2_EVENT		(1<<11)
#define SENS_BUS_ERROR		(1<<13)
#define SENS_CONFIG_ERROR	(1<<14)
#define SENS_SYSTEM_ERROR	(1<<14)

void Sens_Init(void);
void Sens_Deinit(void);

void Sens_eventDetected(uint16 eventToSet);
void Sens_deletEvent(uint16 eventToClear);

uint16 Sens_getEventByte(uint16* ptr);

uint8_t Sens_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);



#endif /* SENSDATA_H_ */
