/*
 * SensData.c
 *
 *  Created on: 26.04.2015
 *      Author: David Schwegler
 */


#include "SensData.h"
#include "UTIL1.h"

static uint16 eventByte;


/*!
 * \brief Prints the help text to the console
 * \param io StdIO handler
 * \return ERR_OK or failure code
 */
static uint8_t Sens_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("Sens", "Sensor Data\r\n", io->stdOut);
  CLS1_SendHelpStr("  help|status", "Print help or status information\r\n", io->stdOut);
  return ERR_OK;
}

/*!
 * \brief Prints the status text to the console
 * \param io StdIO handler
 * \return ERR_OK or failure code
 */
static uint8_t Sens_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[16];
  uint16 dummyByte;
  Sens_getEventByte(&dummyByte);
  CLS1_SendStatusStr("Sens", "\r\n", io->stdOut);
  UTIL1_Num16uToStr(buf, sizeof(buf), dummyByte);
  UTIL1_strcat(buf, sizeof(buf), "\r\n");
  CLS1_SendStatusStr("  EventByte", buf, io->stdOut);
  return ERR_OK;
}

uint8_t Sens_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "Sens help")==0) {
	  *handled = TRUE;
	  return Sens_PrintHelp(io);
  } else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "Sens status")==0) {
	  *handled = TRUE;
	  return Sens_PrintStatus(io);
  }
  return ERR_OK;
}

void Sens_eventDetected(uint16 eventToSet){

	eventByte |= eventToSet;
}


void Sens_deletEvent(uint16 eventToClear){

	eventByte &= ~eventToClear;
}

uint16 Sens_getEventByte(uint16* ptr){
	*ptr = eventByte;
	Sens_deletEvent(SENS_PIR1_EVENT);		/*Bits are cleared after reeding!*/
	Sens_deletEvent(SENS_PIR2_EVENT);
}

void Sens_Init(void){
	eventByte = 0;			//initiate eventByte
}
void Sens_Deinit(void){

}
