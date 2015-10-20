/**
 * \file
 * \brief Non-Volatile memory configuration handling.
 * \author David Schwegler
 *
 * This module implements the infrastructure to store configuration data
 * into non-volatile FLASH memory on the microcontroller.
 */

#include "Platform.h"
#include "NVM_Config.h"
#include "IFsh1.h"
#include "FRTOS1.h"
#include "CS1.h"

static bool isErased(uint8_t *ptr, int nofBytes) {
  while (nofBytes>0) {
    if (*ptr!=0xFF) {
      return FALSE; /* byte not erased */
    }
    ptr++;
    nofBytes--;
  }
  return TRUE;
}

uint8_t NVMC_SaveIdentificationData(void *data, uint16_t dataSize) {

	byte res;
	CS1_CriticalVariable();
	if (dataSize>NVMC_NODE_DATA_SIZE) {
		return ERR_OVERFLOW;
	}
	CS1_EnterCritical();
	res = IFsh1_SetBlockFlash(data, (IFsh1_TAddress)(NVMC_NODE_DATA_START_ADDR), dataSize);
	CS1_ExitCritical();
	return res;
}

void *NVMC_GetIdentifigationeData(void) {
  if (isErased((uint8_t*)NVMC_NODE_DATA_START_ADDR, NVMC_NODE_DATA_SIZE)) {
    return NULL;
  }
  return (void*)NVMC_NODE_DATA_START_ADDR;
}

void NVMC_Init(void) {
  /* nothing needed */
}

void NVMC_Deinit(void) {
  /* nothing needed */
}

