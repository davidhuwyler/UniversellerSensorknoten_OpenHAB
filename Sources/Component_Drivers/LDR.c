/*
 * LDR.c
 *
 *  Created on: 27.04.2015
 *      Author: David Schwegler
 */

#include "LDR.h"
#include "AD1.h"
#include "UTIL1.h"

typedef struct LDR_CALIBRATION{
	uint16 LDR_CALIBRATION;
	uint16 LDR_LAST_MEASURE;
}LDR_CALIBRATION;


#define LDR1_channel 	(0)
#define LDR2_channel	(1)

static LDR_CALIBRATION LDR1;
static LDR_CALIBRATION LDR2;

static void LDR_changeInPercent(uint16 actual, uint16 last,int16 *percent){
	uint16 test = (actual*100/0xFFFF);
	uint16 test2 = (last*100/0xFFFF);
	*percent = test-test2;
}

void LDR1_getCurrentLight(uint16 *ptr,int16 *percent){
	if(AD1_MeasureChan(TRUE,LDR1_channel) == ERR_OK){
		AD1_GetChanValue16(LDR1_channel,ptr);
		//LDR_changeInPercent(*ptr,LDR1.LDR_LAST_MEASURE, percent);  for further implementation change not supportet yet
		LDR1.LDR_LAST_MEASURE = *ptr;
	}
}

void LDR2_getCurrentLight(uint16 *ptr,int16 *percent){
	if(AD1_MeasureChan(TRUE,LDR2_channel) == ERR_OK){
		AD1_GetChanValue16(LDR2_channel,ptr);
		//LDR_changeInPercent(*ptr,LDR2.LDR_LAST_MEASURE, percent);
		LDR2.LDR_LAST_MEASURE = *ptr;
	}
}

/*!
 * \brief Prints the help text to the console
 * \param io StdIO handler
 * \return ERR_OK or failure code
 */
static uint8_t LDR_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("LDR", "LDR Sensor commands\r\n", io->stdOut);
  CLS1_SendHelpStr("  help|status", "Print help or status information\r\n", io->stdOut);
  return ERR_OK;
}


/*!
 * \brief Prints the status text to the console
 * \param io StdIO handler
 * \return ERR_OK or failure code
 */
static uint8_t LDR_PrintStatus(const CLS1_StdIOType *io) {

    uint8_t buf[16];
    uint8_t buf1[16];
    uint16 val1, val2;
    int16 per1, per2;
    LDR1_getCurrentLight(&val1,&per1);
    LDR2_getCurrentLight(&val2,&per2);

    CLS1_SendStatusStr("LDR", "\r\n", io->stdOut);
    UTIL1_Num16uToStr(buf,sizeof(buf),val1);
    UTIL1_strcat(buf, sizeof(buf), "\r\n");
    CLS1_SendStatusStr("  LDR1", buf, io->stdOut);
    //UTIL1_Num16sToStr(buf1,sizeof(buf1),per1);
    //UTIL1_strcat(buf1, sizeof(buf1), "\r\n");
    //CLS1_SendStatusStr("  change", buf1, io->stdOut);
    UTIL1_Num16uToStr(buf,sizeof(buf),val2);
    UTIL1_strcat(buf, sizeof(buf), "\r\n");
    CLS1_SendStatusStr("  LDR2", buf, io->stdOut);
    //UTIL1_Num16sToStr(buf,sizeof(buf),per2);
    //UTIL1_strcat(buf, sizeof(buf), "\r\n");
    //CLS1_SendStatusStr("  change", buf, io->stdOut);
    return ERR_OK;
}

uint8_t LDR_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "LDR help")==0) {
	  *handled = TRUE;
	  return LDR_PrintHelp(io);
  } else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "LDR status")==0) {
	  *handled = TRUE;
	  return LDR_PrintStatus(io);
  }
  return ERR_OK;
}


void LDR_Init(void){
	uint16 dummy;
	LDR1_getCurrentLight(&LDR1.LDR_LAST_MEASURE,&dummy); //used to get a reference Value by initialization, per cent not needed
	LDR2_getCurrentLight(&LDR2.LDR_LAST_MEASURE,&dummy);
}
void LDR_Deinit(void){

}
