/*
 * NodeConfig.h
 *
 *  Created on: 25.04.2015
 *      Author: David Schwegler
 */

#ifndef NODECONFIG_H_
#define NODECONFIG_H_

#include "Platform.h"
#include "CLS1.h"

#define NC_LDR1_MASK	(1<<1)
#define NC_LDR2_MASK	(1<<2)
#define NC_TEMP1_MASK	(1<<3)
#define NC_TEMP2_MASK	(1<<4)
#define NC_RH1_MASK		(1<<5)
#define NC_RH2_MASK		(1<<6)
#define NC_REED1_MASK	(1<<7)
#define NC_REED2_MASK	(1<<8)
#define NC_REED3_MASK	(1<<9)
#define NC_PIR1_MASK	(1<<10)
#define NC_PIR2_MASK	(1<<11)
#define NC_RIR3_MASK	(1<<12)

#define NC_STANDARD_OWN_ID (0xAA)		/*If there is no ID defined for This Node*/
#define NC_STANDARD_MASTER_ID (0xBB)	/*If there is nothing defined this is Standard MAster ID*/
#define NC_STANDARD_CONFIGBYTE (0x04AA)	/*standard Configuration Byte*/

uint16 NC_getConfigData(void);
uint8 NC_setConfigData(uint16 data);

uint8 NC_setConfigBit(uint16 mask);
bool NC_getConfigBit(uint16 mask);

void NC_Init();
void NC_Deinit();

uint8 NC_defineComIDs(uint8 myID, uint8 masterID);
uint8 NC_get_myID(uint8 *ptr);
uint8 NC_get_masterID(uint8 *ptr);

uint8_t NC_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);

#endif /* NODECONFIG_H_ */
