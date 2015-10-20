/*
 * LED.c
 *
 *  Created on: 24.03.2015
 *      Author: dave_sch_6
 */

/*!
** @file LED.c
** @version 01.00
** @brief
**  	LED Interface implementation.
**      This module Interface that make possible to use Code on Robot and FRDM Board.
*/
/*!
**  @addtogroup LED_module LED module documentation
**  @{
*/
/* MODULE Led */

#include "Platform.h"
#include "Shell.h"
#include "CLS1.h"
#include "UTIL1.h"
#include "LED.h"

void LED_StatusToStr(uint8_t *buf, uint8 status);

static uint8_t LED_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("LED", "LED commands\r\n", io->stdOut);
  CLS1_SendHelpStr("  help|status", "Print help or status information\r\n", io->stdOut);
  CLS1_SendHelpStr("  on x", "Turn on LEDx (x number of LED)\r\n", io->stdOut);
  CLS1_SendHelpStr("  off x", "Turn off LEDx (x number of LED)\r\n", io->stdOut);
  return ERR_OK;
}

/*!
 * \brief Prints the status text to the console
 * \param io StdIO handler
 * \return ERR_OK or failure code
 */
static uint8_t LED_PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[16];

  CLS1_SendStatusStr("LED", "\r\n", io->stdOut);
  CLS1_SendStatusStr("  1", LED1_Get() ? "ON\r\n":"OFF\r\n", io->stdOut);
  CLS1_SendStatusStr("  2", LED2_Get() ? "ON\r\n":"OFF\r\n", io->stdOut);
  CLS1_SendStatusStr("  3", LED3_Get() ? "ON\r\n":"OFF\r\n", io->stdOut);
  return ERR_OK;
}



byte LED_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io)
{

	uint32_t val;
	const unsigned char *p;

	if(UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "LED help")==0) {
		*handled = TRUE;
		return LED_PrintHelp(io);
	} else if ((UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0) || (UTIL1_strcmp((char*)cmd, "LED status")==0)) {
		*handled = TRUE;
		return LED_PrintStatus(io);
	}else if (UTIL1_strncmp(cmd, "LED on ", sizeof("LED on ")-1)==0) {
		p = cmd+sizeof("LED on ")-1;
		if (UTIL1_xatoi(&p, &val)==ERR_OK) {
			if(val == 1){
				LED1_On();
			}else if(val==2){
				LED2_On();
			}else if(val==3){
				LED3_On();
			}else{CLS1_SendStatusStr("wrong number (1-3)", "\r\n", io->stdOut);}
			*handled = TRUE;
		}
	}else if (UTIL1_strncmp(cmd, "LED off ", sizeof("LED off ")-1)==0) {
		p = cmd+sizeof("LED off ")-1;
		if (UTIL1_xatoi(&p, &val)==ERR_OK) {
			if(val == 1){
				LED1_Off();
			}else if(val==2){
				LED2_Off();
			}else if(val==3){
				LED3_Off();
			}else{CLS1_SendStatusStr("wrong number (1-3)", "\r\n", io->stdOut);}
		*handled = TRUE;
		}
	}
  return ERR_OK;
}


/*!
 * \brief Initialize all LED.
 */
void LED_Init(void) {
  /* all LED's off by default */

	LED1_Off();
	LED2_Off();
	LED3_Off();
}

void LED_Deinit(void) {
  /* nothing to do */
}

/*@}
 */


