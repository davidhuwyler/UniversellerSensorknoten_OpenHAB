/*
 * LDR.h
 *
 *  Created on: 27.04.2015
 *      Author: David Schwegler
 */

#ifndef LDR_H_
#define LDR_H_

#include "Platform.h"
#include "CLS1.h"

void LDR_Init(void);
void LDR_Deinit(void);

void LDR1_getCurrentLight(uint16 *ptr,int16 *percent);
void LDR2_getCurrentLight(uint16 *ptr,int16 *percent);

uint8_t LDR_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);


#endif /* LDR_H_ */
