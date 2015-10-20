/*
 * Config.c
 *
 *  Created on: 25.04.2015
 *      Author: David Schwegler
 */

#include "NodeConfig.h"
#include "NVM_Config.h"
#include "UTIL1.h"
#include "Shell.h"


typedef struct IdentificationT_{	/*! ID's for communication over RS485*/
	uint8_t MyID;
	uint8_t MasterID;
	uint16_t SensorConfiguration;
} NC_NodeIdentificationData;

NC_NodeIdentificationData NodeData;


bool NC_getConfigBit(uint16 mask){
	NC_NodeIdentificationData *ptr = NVMC_GetIdentifigationeData();
	uint16_t val;
	if(ptr != NULL){
		val = ptr->SensorConfiguration;
		val = val & mask;
		if(val == mask){
			return TRUE;
		}else{
			return FALSE;
		}
	}

}

uint8 NC_setConfigBit(uint16 mask){
#if PL_HAS_NVM
	NC_NodeIdentificationData *local = NVMC_GetIdentifigationeData();
		if(local != NULL){
			NodeData = *local;
			NodeData.SensorConfiguration |= mask;
			if(NVMC_SaveIdentificationData(&NodeData,sizeof(NodeData)) == ERR_OK){
				return ERR_OK;
			}
	}
	return ERR_FAILED;
#else
	configData |=  mask;
#endif

}

uint16 NC_getConfigData(void){
#if PL_HAS_NVM
	NC_NodeIdentificationData *ptr =NVMC_GetIdentifigationeData();
	if(ptr != NULL){
		return ptr->SensorConfiguration;
	}
	return ERR_FAILED;
#endif
}

uint8 NC_setConfigData(uint16 confdata){
	NC_NodeIdentificationData *ptr = NVMC_GetIdentifigationeData();		/* load Data from Flash*/
	if(ptr != NULL){
		NodeData = *ptr;
		NodeData.SensorConfiguration = confdata;
		if(NVMC_SaveIdentificationData(&NodeData,sizeof(NodeData)) == ERR_OK){
			return ERR_OK;
		}
	}
	return ERR_FAILED;
}

/*!
 * \brief 	Read Communication Data from Flash
 * \param 	void
 *
 * \return 	Configurated Node ID or OxFF if there is a Problem while reding Data.
 */
uint8 NC_get_myID(uint8 *ptr){
#if PL_HAS_NVM
	NC_NodeIdentificationData *local = NVMC_GetIdentifigationeData();
	if(local != NULL){
		*ptr = local->MyID;
		return ERR_OK;
	}
	return ERR_FAILED;
#else
	return CommID.MyID;
#endif
}


/*!
 * \brief 	Read Master ID from Flash and returns this Value
 * \param 	void
 *
 * \return 	Configurated Master ID or OxFF if there is a Problem while reding Data.
 */
uint8 NC_get_masterID(uint8 *ptr){
#if PL_HAS_NVM
	NC_NodeIdentificationData *local = NVMC_GetIdentifigationeData();
	if(local != NULL){
		*ptr = local->MasterID;
		return ERR_OK;
	}
	return ERR_FAILED;
#else
	return CommID.MasterID;
#endif
}

uint8 NC_defineComIDs(uint8 myID, uint8 masterID){
#if PL_HAS_NVM
	NC_NodeIdentificationData *local = NVMC_GetIdentifigationeData();		/* load Data from Flash*/
	if(local != NULL){
		NodeData = *local;
		if(myID != 0){
			NodeData.MyID = myID;
		}if(masterID != 0){
			NodeData.MasterID = masterID;
		}
		if(NVMC_SaveIdentificationData(&NodeData,sizeof(NodeData)) == ERR_OK){
			return ERR_OK;
		}
	}
	return ERR_FAILED;
#else
	CommID.MasterID = masterID;
	return ERR_OK;
#endif
}

/*!
 * \brief Prints the help text to the console
 * \param io StdIO handler
 * \return ERR_OK or failure code
 */
static uint8_t NC_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("NC", "NC Node Configuration commands\r\n", io->stdOut);
  CLS1_SendHelpStr("  help|status", "Print help or status information\r\n", io->stdOut);
  CLS1_SendHelpStr("  id config <node> <master>", (unsigned char*)"Set new Communication ID's (in Hex) \r\n", io->stdOut);
  CLS1_SendHelpStr("  set ConfigByte <dec>", "enable/disable connected Sensors\r\n", io->stdOut);
  return ERR_OK;
}


/*!
 * \brief Prints the status text to the console
 * \param io StdIO handler
 * \return ERR_OK or failure code
 */
static uint8_t NC_PrintStatus(const CLS1_StdIOType *io) {

    uint8_t buf[16];
    uint8_t buf1[8];
    uint8_t buf2[8];
    uint8 myID_copy;
    uint8 masterID_copy;
    uint16 val1;

    CLS1_SendStatusStr("NC", "\r\n", io->stdOut);
    buf1[0] = '\0';
    buf2[0] = '\0';
    if(NC_get_myID(&myID_copy)==ERR_OK && NC_get_masterID(&masterID_copy)==ERR_OK){
    	UTIL1_strcatNum8Hex(buf1,sizeof(buf1),myID_copy);
		UTIL1_strcat(buf1, sizeof(buf1), "\r\n");
		CLS1_SendStatusStr("  Node ID", buf1, io->stdOut);
		UTIL1_strcatNum8Hex(buf2,sizeof(buf2),masterID_copy);
		UTIL1_strcat(buf2, sizeof(buf2), "\r\n");
		CLS1_SendStatusStr("  Master ID", buf2, io->stdOut);
		UTIL1_Num16uToStr(buf,sizeof(buf),NC_getConfigData());
		UTIL1_strcat(buf, sizeof(buf), "\r\n");
		CLS1_SendStatusStr("  ConfigByte", buf, io->stdOut);
    }else{
    	CLS1_SendStatusStr("  Node ID", "error FLASH \r\n", io->stdOut);
    	CLS1_SendStatusStr("  Master ID", "error FLASH \r\n", io->stdOut);
    	CLS1_SendStatusStr("  ConfigByte", "error FLASH \r\n", io->stdOut);
    }
    return ERR_OK;
}

uint8_t NC_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
	const unsigned char *p;
	const unsigned char *f;
	uint16 val;
	uint8_t val1;
	uint8_t val2;


  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "NC help")==0) {
	  *handled = TRUE;
	  return NC_PrintHelp(io);
  } else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "NC status")==0) {
	  *handled = TRUE;
	  return NC_PrintStatus(io);
  }else if (UTIL1_strncmp(cmd, "NC set ConfigByte ", sizeof("NC set ConfigByte ")-1)==0) {
	  p = cmd+sizeof("NC set CofigByte ")-1;
	  if(UTIL1_ScanDecimal16uNumber(&p,&val)==ERR_OK) {
		 NC_setConfigData(val);
		  *handled = TRUE;
	  } else {
		  SHELL_SendString((unsigned char*)"No value found! Please enter decimal\r\n");
		  return ERR_FAILED; /* wrong format of command? */
	  }
  }else if (UTIL1_strncmp(cmd, "NC id config ", sizeof("NC id config ")-1)==0) {
	  p = cmd+sizeof("NC id config ")-1;
	  f = p + sizeof("0xFF ")-1;
	  if(UTIL1_ScanHex8uNumber(&p,&val1)==ERR_OK && UTIL1_ScanHex8uNumber(&f,&val2)==ERR_OK) {
		  NC_defineComIDs(val1,val2);
		  *handled = TRUE;
	  } else {
		  SHELL_SendString((unsigned char*)"ID should be entered in Hex Format 0x...\r\n");
		  return ERR_FAILED; /* wrong format of command? */
	  }
  }
  return ERR_OK;
}


void NC_Init(void){
#if PL_HAS_NVM
	NC_NodeIdentificationData *local = NVMC_GetIdentifigationeData();		/* load Data from Flash*/

	if(local == NULL){
		NC_NodeIdentificationData initialData;
		SHELL_SendString("No Data available in FLASH: \r\n");
		initialData.MyID = NC_STANDARD_OWN_ID;
		initialData.MasterID = NC_STANDARD_MASTER_ID;
		initialData.SensorConfiguration = NC_STANDARD_CONFIGBYTE;
		if(NVMC_SaveIdentificationData(&initialData,sizeof(initialData)) == ERR_OK){
			SHELL_SendString("FLASH Initiaized\r\n");
			SHELL_SendString("  NODE   ID: 0xAA\r\n");
			SHELL_SendString("  MASTER ID: 0xBB\r\n");
			SHELL_SendString("  CONFIGBYTE: 0x4AA\r\n");
		}
	}

#else
	#if PL_IS_MASTER
		CommID.MyID = RS_TEST_MASTER_ID;
		CommID.MasterID = RS_TEST_SLAVE_ID;
	#else
		CommID.MyID = RS_TEST_SLAVE_ID;
		CommID.MasterID = RS_TEST_MASTER_ID;
	#endif
		NC_defineComIDs(NC_STANDARD_OWN_ID,NC_STANDARD_MASTER_ID); //default Settings
		NC_setConfigBit(NC_LDR1_MASK);
		NC_setConfigBit(NC_TEMP1_MASK);
		NC_setConfigBit(NC_RELAIS1_MASK);
		NC_setConfigBit(NC_REED1_MASK);
		NC_setConfigBit(NC_PIR1_MASK);
#endif
}


void NC_Deinit(){

}
