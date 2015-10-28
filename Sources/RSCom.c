/*
 * RSCom.c
 *
 *  Created on: 10.04.2015
 *      Author: David Schwegler
 */


#include "Platform.h"
#include "string.h"
#include "RSCom.h"
#include "WAIT1.h"
#include "FRTOS1.h"
#include "AS1.h"
#include "RTS.h"
#include "RNG1.h"
#include "Component_Drivers/LED.h"
#include "Component_Drivers/LDR.h"
#include "Sem.h"
#include "NVM_Config.h"
#include "Shell.h"
#include "NodeConfig.h"
#include "SensData.h"

#define RS_DUMMY 	(0x55)			/*Dummy Byte 0x55 displays start of Communication*/
#define RS_STX 		(0x02)			/*Start of Text" marks start of Ticket */
#define RS_ETX 		(0x03)			/*End of Text" marks end of Ticket */


#define RS_TEST_MASTER_ID  (0x3F)			/*Dummy Values for Communication tests*/
#define RS_TEST_SLAVE_ID  (0x2E)			/*Dummy Values for Communication tests*/
#define RS_TEST_VAL1 (0x0023)
#define RS_TEST_VAL2 (0x2F34)
#define RS_TEST_VAL3 (0x3F45)

#define START_OF_DUMMY 	(0)
#define START_OF_STX 	(1)
#define START_OF_ID 	(START_OF_STX + 1)
#define START_OF_VAL1 	(START_OF_ID + 2)
#define START_OF_VAL2 	(START_OF_VAL1 + 4)
#define START_OF_VAL3 	(START_OF_VAL2 + 4)
#define START_OF_ETX 	(START_OF_VAL3 + 4)

#define RS_ERROR_REPORT (0xE)

#define NOF_UINT16_IN_PACKAGE	7

#define NOF_MAX_SLAVES	(128)

typedef struct{
	uint8 high	:4;
	uint8 high_i:4;
	uint8 low	:4;
	uint8 low_i :4;
}NIBBLE;

typedef struct{						/* Interesting date from a Package for Application level*/
	uint16 VAL1;
	uint16 VAL2;
	uint16 VAL3;
}TICKET_DATA;

typedef struct{						/*! Data Package for RS485 Communication for MAC level. Every nibble inverted added*/
	uint16 REPORT_ID;
	uint16 VAL1_H;
	uint16 VAL1_L;
	uint16 VAL2_H;
	uint16 VAL2_L;
	uint16 VAL3_H;
	uint16 VAL3_L;
}TICKET_PKG;

typedef struct{						/*! Ticket for RS485 Communication for physical Layer*/
	uint8 DUMMY;
	uint8 STX;
	TICKET_PKG PKG;
	uint8 ETX;
}TICKET_RAW;

#if PL_HAS_OPENHAB 		//ListEntry for the EventBytes of the slaves
typedef struct{
	bool slaveInUse;	//The Slave witch is polled at the moment
	bool slaveToPoll;	//All active Slaves
	uint16 oldVal;		//Old EventByte
	uint16 newVal;		//New EventByte
}LIST_ENTRY;

#endif

typedef enum {						/*States for RS_485 transmission State Machine*/
  RS_STATE_IDLE,
  RS_STATE_SYC_ON_BEGIN,
  RS_STATE_PARSE_TICKET,
  RS_STATE_PARSE_COMMAND,
  RS_STATE_ANSWER_TICKET,
  NOF_STATES
} RS_StateType;

//System Commands
#define SYS_GET_EVENTBYTE 		(1)
#define SYS_SET_EVENTBYTE 		(2)
#define SYS_GET_CONFIGBYTE  	(3)
#define SYS_SET_CONFIGBYTE 		(4)
#define SYS_BUS_STABILITY_TEST 	(5)
//LDR Commands
#define LDR1_GET_CURRENT_LIGHT 	(11)
#define LDR1_GET_CALIB_DATA		(12)
#define LDR1_WRITE_CALIBRATION	(13)
#define LDR2_GET_CURRENT_LIGHT 	(16)
#define LDR2_GET_CALIB_DATA		(17)
#define LDR2_WRITE_CALIBRATION	(18)
//Temperature Commands
#define TEMP1_GET_TEMP 			(21)
#define TEMP1_GET_HUMIDITY 		(22)
#define TEMP2_GET_TEMP 			(26)
#define TEMP2_GET_HUMIDITY 		(27)
//Relais Commands
#define REL1_GET_STATUS			(31)
#define REL1_SET_STATUS			(32)
#define REL2_GET_STATUS			(36)
#define REL2_SET_STATUS			(37)
//Reed Commands
#define REED1_GET_STATUS		(41)
#define REED2_GET_STATUS		(46)
//PIR Commands
#define PIR1_GET_STATUS			(51)
#define PIR2_GET_STATUS			(55)
//LED Commands
#define LED2_TOGGLE				(61)
#define LED3_TOGGLE				(71)

static bool requestHandled;
static RS_StateType refState; /* state machine state */

#if PL_IS_MASTER
static TICKET_DATA masterSendTicket;
static bool sendRepeated;
#endif

#if PL_HAS_OPENHAB //Liste mit Slaves und deren Eventbytes
static LIST_ENTRY eventByteList[NOF_MAX_SLAVES]; //Pro Slave ein Listeneintrag
static xQueueHandle eventByteQueue; //Queue für The EventByte Entry
#endif


/*!
 * \brief 	Compares the nibbles of each Byte.
 * \param 	*ptr pointer to uint8 value
 *
 * \return 	TRUE if high-nibble is same as inverted low-nibble. FALSE otherwise
 */
static bool RS_removeDataIntegrity(uint8 arr[], uint8 length)
{
	NIBBLE nibble;
	for(int i = START_OF_ID;i<length-1;i++){
		nibble.high =  (arr[i] >> 4);
		nibble.low =  ~arr[i];

		if(nibble.low != nibble.high){
			return FALSE;
		}else{
			arr[i] = nibble.high;
		}
	}

	return TRUE;
}


/*!
 * \brief 	Combine two 4 bit Values in an Byte
 * \param 	high uint8 that contains the higher Nibble
 * 			low  uint8 that contains the lower Nibble
 *
 * \return 	Combined 8 Bit Value
 */
static uint8 combineByte(uint8 high, uint8 low)
{
	return ((high << 4) | low);
}


/*!
 * \brief 	Combine Data out of 4 Bytes to one uint16
 * \param 	*ptr pointer data array
 *
 * \return 	uint 16 Value
 */
static uint16 RS_extractI16(uint8 arr[])
{
	uint8 high = combineByte(arr[0],arr[1]);
	uint8 low = combineByte(arr[2],arr[3]);

	return ((high << 8) | low);
}


/*!
 * \brief 	extract a Ticket out of the ringBuffer in a RS_TICKET struct
 * \param 	*ticket pointer to struct
 *
 */
static uint8 RS_unpackTicket(TICKET_DATA *cmd)
{
	uint8 buf[NOF_BYTES_IN_TICKET];

	for(int i = 0;i<NOF_BYTES_IN_TICKET;i++){
		RNG1_Get(&buf[i]);
	}

	if(RS_removeDataIntegrity(buf,sizeof(buf)) == FALSE){
		Sens_eventDetected(SENS_BUS_ERROR);
		return ERR_COMMON;
	}

	uint8 id;
	if(NC_get_myID(&id)!=ERR_OK){
		id = NC_STANDARD_OWN_ID;
	}

	if(combineByte(buf[START_OF_ID],buf[START_OF_ID+1]) != id){		//is Ticket addressed at me?
		return ERR_FAILED;
	}


	cmd->VAL1 = RS_extractI16(&buf[START_OF_VAL1]);
	cmd->VAL2 = RS_extractI16(&buf[START_OF_VAL2]);
	cmd->VAL3 = RS_extractI16(&buf[START_OF_VAL3]);

	return ERR_OK;
}

/*!
 * \brief Creates a Ticket for RS485 Communication. Add Dummy Byte Start- and Stop Byte
 * \param 	*ticket pointer to struct where Data is stored
 * 			*buf	pointer to local buffer with received data
 * 			legth	bytes in local buffer
 *
 */
static uint8 RS_syncOnBegin(void){

	uint8 dummy = 0;
	uint8 stx = 0;
	uint8 etx = 0;

	while(RNG1_NofElements() >= NOF_BYTES_IN_TICKET){
		uint8 delete;

		RNG1_Peek(START_OF_DUMMY,&dummy);
		RNG1_Peek(START_OF_STX,&stx);
		RNG1_Peek(START_OF_ETX,&etx);

		if(dummy == RS_DUMMY && stx == RS_STX && etx == RS_ETX){
			return ERR_OK;
		}
		RNG1_Get(&delete);
	}
	return ERR_FAILED;
}

static bool RS_isSensorAvaliable(uint16 command){

	if(command >= LDR1_GET_CURRENT_LIGHT && command < LDR2_GET_CURRENT_LIGHT){
		return NC_getConfigBit(NC_LDR1_MASK);
	}else if(command >= LDR2_GET_CURRENT_LIGHT && command < TEMP1_GET_TEMP){
		return NC_getConfigBit(NC_LDR2_MASK);
	}else if(command >= TEMP1_GET_TEMP && command < TEMP2_GET_TEMP){
		return NC_getConfigBit(NC_TEMP1_MASK);
	}else if(command >= TEMP2_GET_TEMP && command < REL1_GET_STATUS){
		return NC_getConfigBit(NC_TEMP2_MASK);
	}else if(command >= REL1_GET_STATUS && command < REL2_GET_STATUS){
		return NC_getConfigBit(NC_RH1_MASK);
	}else if(command >= REL2_GET_STATUS && command < REED1_GET_STATUS){
		return NC_getConfigBit(NC_RH2_MASK);
	}else if(command == REED1_GET_STATUS ){
		return NC_getConfigBit(NC_REED1_MASK);
	}else if(command == REED2_GET_STATUS){
		return NC_getConfigBit(NC_REED2_MASK);
	}else if(command == PIR1_GET_STATUS){
		return NC_getConfigBit(NC_PIR1_MASK);
	}else if(command == PIR2_GET_STATUS){
		return NC_getConfigBit(NC_PIR2_MASK);
	}
	return TRUE;
}

static void RS_executeCmd(TICKET_DATA *cmd,TICKET_DATA *report){
	uint16 command = cmd->VAL1;
	uint16 val1 = 0;
	uint16 val2 = 0;
	uint16 val3 = 0;
	uint8_t buf[30];
	BaseType_t res;

#if PL_IS_MASTER

#if PL_HAS_OPENHAB //Prüfung ob sich Eventbyte änderte

	res =FRTOS1_xQueueSendToBack(eventByteQueue,&command,200/portTICK_RATE_MS);
	if(res != pdPASS)
	{
		SHELL_SendString("Queue Full!/r/n");
	}
#else

	buf[0] = '\0';
	UTIL1_strcat(buf, sizeof(buf), "rec. Value1: ");
	UTIL1_strcatNum16Hex(buf,sizeof(buf),command);
	UTIL1_strcat(buf, sizeof(buf), "\r\n");
	SHELL_SendString(buf);
	buf[0] = '\0';
	UTIL1_strcat(buf, sizeof(buf), "rec. Value2: ");
	UTIL1_strcatNum16Hex(buf,sizeof(buf),cmd->VAL2);
	UTIL1_strcat(buf, sizeof(buf), "\r\n");
	SHELL_SendString(buf);
#endif
#else
	if(RS_isSensorAvaliable(command)){
		switch(command){
			case SYS_GET_EVENTBYTE:
				Sens_getEventByte(&val1);
				break;
			case SYS_GET_CONFIGBYTE:
				val1 = NC_getConfigData();
				break;
			case SYS_SET_CONFIGBYTE:
				NC_setConfigData(cmd->VAL2);
				break;
			case SYS_BUS_STABILITY_TEST:
				val1 = RS_DUMMY;
				val2 = RS_DUMMY;
				break;
			case LDR1_GET_CURRENT_LIGHT:
				LDR1_getCurrentLight(&val1,&val2);
				break;
			case LDR1_GET_CALIB_DATA:
				break;
			case LDR1_WRITE_CALIBRATION:
				break;
			case LDR2_GET_CURRENT_LIGHT:
				LDR2_getCurrentLight(&val1,&val2);
				break;
			case LDR2_GET_CALIB_DATA:
				break;
			case LDR2_WRITE_CALIBRATION:
				break;
			case TEMP1_GET_TEMP:
				break;
			case TEMP1_GET_HUMIDITY:
				break;
			case TEMP2_GET_TEMP:
				break;
			case TEMP2_GET_HUMIDITY:
				break;
			case REL1_GET_STATUS:
				break;
			case REL1_SET_STATUS:
				break;
			case REL2_GET_STATUS:
				break;
			case REL2_SET_STATUS:
				break;
			case REED1_GET_STATUS:
				break;
			case REED2_GET_STATUS:
				break;
			case PIR1_GET_STATUS:
				val1 = PIR1_GET_STATUS;
				break;
			case PIR2_GET_STATUS:
				val1 = PIR2_GET_STATUS;
				break;
			case LED2_TOGGLE:
				LED2_Neg();
				break;
			case LED3_TOGGLE:
				LED3_Neg();
				break;
			default:
				break;
		}
	}else{
		Sens_eventDetected(SENS_CONFIG_ERROR);
		val1 = RS_ERROR_REPORT;
		val2 = command << 1;
	}
#endif
	report->VAL1 = val1;
	report->VAL2 = val2;
	report->VAL3 = val3;
}

/*!
 * \brief  add data integrity (invert every nibble)
 * \param 	*data pointer data
 *
 * \return 	data with integrity
 */
static uint16 RS_data_integrity(uint8 *data){

	NIBBLE nibble;
	nibble.high =  (*data >> 4);
	nibble.low =  (*data);

	nibble.high_i = (~nibble.high);
	nibble.low_i = (~nibble.low);

	uint8 high = (nibble.high << 4) | (nibble.high_i);
	uint8 low = (nibble.low << 4) | (nibble.low_i);

	return ((low << 8) | high);		/*Bytes switched because of little endian architecture*/
}

static void RS_i16_To_2i16(uint16 *data, uint16 *highbyte, uint16 *lowbyte){

	uint8 high_8 = (uint8) (*data >> 8);
	uint8 low_8 = (uint8) (*data);

	*highbyte = RS_data_integrity(&high_8);
	*lowbyte = RS_data_integrity(&low_8);
}

/*!
 * \brief Creates a Ticket for RS485 Communication. Add Dummy Byte Start- and Stop Byte
 * \param 	*ticket pointer to ticket struct
 * 			Val_x	Report Value 1,2,3
 *
 */
static void RS_CommandTOPackage(TICKET_DATA *cmd, TICKET_PKG *pkg)
{

	uint8 id;
	if(NC_get_masterID(&id) != ERR_OK){
		id = NC_STANDARD_MASTER_ID;
	}

	pkg->REPORT_ID = RS_data_integrity(&id);			//ID of Bus Master

	RS_i16_To_2i16(&(cmd->VAL1),&(pkg->VAL1_H),&(pkg->VAL1_L));
	RS_i16_To_2i16(&(cmd->VAL2),&(pkg->VAL2_H),&(pkg->VAL2_L));
	RS_i16_To_2i16(&(cmd->VAL3),&(pkg->VAL3_H),&(pkg->VAL3_L));

}

/*!
 * \brief 	Send a Ticket over RS485 Communication Bus. Adds start, Stop and safety Conditions
 * 			for Communication.
 *
 * \param 	*ticket pointer to data to transmit
 * 			Val_x	Report Value 1,2,3
 *
 */
static uint8 RS_SendTicket(TICKET_DATA *cmd)
{
	uint16 bytesSend;
	TICKET_RAW ticket;
	TICKET_RAW *ticketptr;
	ticketptr = &ticket;
	TICKET_PKG pkg;

	RS_CommandTOPackage(cmd,&pkg);

	ticket.DUMMY = RS_DUMMY;
	ticket.STX = RS_STX;
	ticket.PKG = pkg;
	ticket.ETX = RS_ETX;

#if PL_IS_MASTER
	//AS1_SendChar(RS_STX);
	//AS1_SendChar(RS_DUMMY);
	//AS1_SendChar(RS_STX);
#endif
	AS1_TComData sendBuf[NOF_BYTES_IN_TICKET];
	memcpy(sendBuf,&ticket,NOF_BYTES_IN_TICKET);
	if(AS1_SendBlock(sendBuf,(NOF_BYTES_IN_TICKET),&bytesSend)==ERR_OK){
		if(bytesSend == NOF_BYTES_IN_TICKET){
			return ERR_OK;
		}
	}
	return ERR_FAILED;
}

uint8 RS_SendTicketBlocking(TICKET_DATA *cmd,uint16 *data)
{
	uint8_t res;
	FRTOS1_taskENTER_CRITICAL();
	FRTOS1_xQueueReceive(eventByteQueue,data,0); //Queue leeren
	res = RS_SendTicket(cmd);
	FRTOS1_taskEXIT_CRITICAL();
	if(res != ERR_OK)
	{
		*data=0x0000;
		return res;
	}

	if(FRTOS1_xQueueReceive(eventByteQueue, data, 10/portTICK_RATE_MS)!=pdPASS)
	{
		*data=0x0000;
		return ERR_NOTAVAIL;
	}

	return ERR_OK;
}

static void RSCom_StateMachine(void){
	TICKET_DATA cmd;
	TICKET_DATA report;
	requestHandled = FALSE;

	while(!requestHandled){
		switch(refState){
			case RS_STATE_IDLE:
				refState = RS_STATE_SYC_ON_BEGIN;
				break;
			case RS_STATE_SYC_ON_BEGIN:
				if(RS_syncOnBegin() == ERR_FAILED){
					requestHandled = TRUE;
					refState = RS_STATE_IDLE;
				}else{
					refState = RS_STATE_PARSE_TICKET;
				}
				break;
			case RS_STATE_PARSE_TICKET:
				if(RS_unpackTicket(&cmd)== ERR_OK){
					refState = RS_STATE_PARSE_COMMAND;
				}else{
					requestHandled = TRUE;
					refState = RS_STATE_IDLE;
				}
				break;
			case RS_STATE_PARSE_COMMAND:
				RS_executeCmd(&cmd, &report);
				refState = RS_STATE_ANSWER_TICKET;
				break;
			case RS_STATE_ANSWER_TICKET:
#if !PL_IS_MASTER
				RS_SendTicket(&report);
#endif
				requestHandled = TRUE;
				refState = RS_STATE_IDLE;
				break;
		}
	}
}

/*!
 * \brief Prints the help text to the console
 * \param io StdIO handler
 * \return ERR_OK or failure code
 */
static uint8_t RS_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr("RS", "RS Communicaion commands\r\n", io->stdOut);
  CLS1_SendHelpStr("  help|status", "Print help or status information\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  set command <val1>", (unsigned char*)"Set command for communication \r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  send <ID> <Cmd> <val1>", (unsigned char*)"Send a command to an slave (in Hex)\r\n", io->stdOut);
#if PL_HAS_OPENHAB
  CLS1_SendHelpStr((unsigned char*)"  add Slave <ID>", (unsigned char*)"Add a Slave to poll the EventByte\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  remove Slave <ID>", (unsigned char*)"Remove a Slave to poll the EventByte\r\n", io->stdOut);
#else
  CLS1_SendHelpStr((unsigned char*)"  repeat cmd (on/off)", (unsigned char*)"Send command every 3 Seconds\r\n", io->stdOut);
#endif
  return ERR_OK;
}

/*!
 * \brief Prints the status text to the console
 * \param io StdIO handler
 * \return ERR_OK or failure code
 */
static uint8_t RS_PrintStatus(const CLS1_StdIOType *io) {

  CLS1_SendStatusStr("RS", "\r\n", io->stdOut);

  return ERR_OK;
}

uint8_t RS_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {

	const unsigned char *p;
	const unsigned char *f;
	const unsigned char *k;
	uint8_t val1;
	uint8_t val2;
	uint16_t val3;

  if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, "RS help")==0) {
	  *handled = TRUE;
	  return RS_PrintHelp(io);
  } else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, "RS status")==0) {
	  *handled = TRUE;
	  return RS_PrintStatus(io);
  }
#if PL_HAS_OPENHAB
  else if (UTIL1_strncmp(cmd, "RS add Slave ", sizeof("RS add Slave ")-1)==0)
  {
    	  p = cmd+sizeof("RS add Slave ")-1;
    	  if(UTIL1_ScanHex8uNumber(&p,&val1)==ERR_OK)
    	  {
    		  eventByteList[val1].slaveToPoll = TRUE;
    		  *handled = TRUE;
    	  }
  }
  else if (UTIL1_strncmp(cmd, "RS remove Slave ", sizeof("RS remove Slave ")-1)==0)
  {
    	  p = cmd+sizeof("RS remove Slave ")-1;
    	  if(UTIL1_ScanHex8uNumber(&p,&val1)==ERR_OK)
    	  {
    		  eventByteList[val1].slaveToPoll = FALSE;
    		  *handled = TRUE;
    	  }
  }
#endif
#if PL_IS_MASTER
  else if (UTIL1_strncmp(cmd, "RS set command ", sizeof("RS set command ")-1)==0) {
	  p = cmd+sizeof("RS set command ")-1;
	  if(UTIL1_ScanHex8uNumber(&p,&val1)==ERR_OK) {
		  masterSendTicket.VAL1 = val1;
		  *handled = TRUE;
	  } else {
		  SHELL_SendString((unsigned char*)"ID should be entered in Hex Format 0x...\r\n");
		  return ERR_FAILED; /* wrong format of command? */
	  }
  	 }else if (UTIL1_strncmp(cmd, "RS send ", sizeof("RS send ")-1)==0) {
  	  p = cmd+sizeof("RS send ")-1;
  	  f = p + sizeof("0xFF ")-1;
  	  k = f + sizeof("0xFF ")-1;
  	  if(UTIL1_ScanHex8uNumber(&p,&val1)==ERR_OK && UTIL1_ScanHex8uNumber(&f,&val2)==ERR_OK && UTIL1_ScanHex16uNumber(&k,&val3)==ERR_OK) {
  		  NC_defineComIDs(0,val1);
  		  masterSendTicket.VAL1 = val2;
  		  masterSendTicket.VAL2 = val3;
  		  RS_SendTicket(&masterSendTicket);
  		  *handled = TRUE;
  	  }
    }else if (UTIL1_strcmp((char*)cmd, (char*)"RS repeat cmd on")==0) {
    	sendRepeated = TRUE;
    	*handled = TRUE;
    }else if (UTIL1_strcmp((char*)cmd, (char*)"RS repeat cmd off")==0) {
    	sendRepeated = FALSE;
    	*handled = TRUE;
    }
#endif
  return ERR_OK;
}




/*!
 * \brief Reads Elements from RS-485 Communication Buffer and puts them into local ringbuffer.
 * \param 	void
 *
 *
 */
static byte fillLocalBuffer(void){
	int i = 0;
	char chr = 0;

	for(i = 0; i < NOF_BYTES_IN_TICKET; i++){
		if(AS1_RecvChar(&chr) == ERR_RXEMPTY){
			return ERR_FAILED;
		}
		RNG1_Put(chr);
	}
	return ERR_OK;
}

static void Task_RS_ReadWrite (void *pvParams)
{
	(void)pvParams;

	for(;;){
		if(SEM_takeRS_sem()){
			if(fillLocalBuffer()==ERR_OK){
				RSCom_StateMachine();
			}
		}
	}
}

#if PL_HAS_OPENHAB
static uint8 FindSlaveInUse (LIST_ENTRY *array)
{
	uint8 i;
	for(i=0;i<NOF_MAX_SLAVES;i++)
	{
		if(array[i].slaveInUse)
		{
			return i;
		}
	}
	return -1; //No slave aktiv ->Error
}

static uint8 FindNextSlave (LIST_ENTRY *array,uint8 currentPos)
{
	uint8 j = currentPos+1;
	while(j<=NOF_MAX_SLAVES-1) //Search for next slave above the actual slave
	{
		if(array[j].slaveToPoll)
		{
			return j;
		}
		j++;
	}
	j = 0;
	while(j<=currentPos) //Search for slave under the actual slave
	{
		if(array[j].slaveToPoll)
		{
			return j;
		}
		j++;
	}

	return currentPos; //Just one slave in the system

}
#endif

#if PL_IS_MASTER
static void Task_RS_Send (void *pvParams){

	(void)pvParams;

	for(;;){
		if(sendRepeated){


#if PL_HAS_OPENHAB //Polling der Eventbytes der Slaves
	masterSendTicket.VAL1 = 0x01;
	masterSendTicket.VAL2 = 0x00;
	uint16 newEventByte;

	uint8 lastSlave = FindSlaveInUse(eventByteList);
	uint8 currentSlave = FindNextSlave(eventByteList,lastSlave);
	uint8 res;

	eventByteList[lastSlave].slaveInUse = FALSE;
	eventByteList[currentSlave].slaveInUse = TRUE;
	NC_defineComIDs(0,currentSlave);
	res=RS_SendTicketBlocking(&masterSendTicket,&newEventByte);
	if(res == ERR_OK)
	{
		eventByteList[currentSlave].newVal = newEventByte;

		if(res = eventByteList[currentSlave].oldVal != eventByteList[currentSlave].newVal)
		{
			uint8_t buf_slave[3];
			uint8_t buf[30];

			buf[0] = '\0';
			UTIL1_strcat(buf, sizeof(buf), "Events_");
			UTIL1_Num8uToStr(buf_slave,sizeof(buf_slave),currentSlave);
			UTIL1_strcat(buf, sizeof(buf), buf_slave);
			UTIL1_strcat(buf, sizeof(buf), ": ");
			UTIL1_strcatNum16Hex(buf,sizeof(buf),newEventByte);
			UTIL1_strcat(buf, sizeof(buf), "\r\n");
			SHELL_SendString(buf);
			eventByteList[currentSlave].oldVal = eventByteList[currentSlave].newVal;
		}

	}
	else if(res == ERR_NOTAVAIL)
	{
		SHELL_SendString("TimeOut\r\n");
	}
	else
	{
		SHELL_SendString("Error SendTicketBlocking\r\n");
	}

#else
			if(RS_SendTicket(&masterSendTicket) == ERR_OK)
			{
				LED3_Neg();
			}
#endif
		}
#if PL_HAS_OPENHAB
		FRTOS1_vTaskDelay(100/portTICK_RATE_MS);
#else
		FRTOS1_vTaskDelay(3000/portTICK_RATE_MS);
#endif
	}
}
#endif

void RS_Init(void){
	RTS_ClrVal();			//Set Node as Listener
	RNG1_Init();			//Initiate internal Ring Buffer

	if (FRTOS1_xTaskCreate(Task_RS_ReadWrite, "RSTask", configMINIMAL_STACK_SIZE+100, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
		for(;;){} /* error */
	}
#if PL_IS_MASTER

#if PL_HAS_OPENHAB //Init der EventByteList

	eventByteQueue = FRTOS1_xQueueCreate(1,2);

	uint8 i;
	for (i=0; i<NOF_MAX_SLAVES; i++)
	{
		eventByteList[i].slaveInUse = FALSE;
		eventByteList[i].slaveToPoll = FALSE;
		eventByteList[i].newVal = 0xFFFF;
		eventByteList[i].oldVal = 0xFFFF;
	}
	//Erster Slave zu Pollen
	eventByteList[0x22].slaveInUse = TRUE;

	//Liste mit allen zu Pollenden Slaves
	eventByteList[0x22].slaveToPoll = TRUE;
	sendRepeated = TRUE;

#else
	sendRepeated = FLASE;
#endif
	LED3_On();
	masterSendTicket.VAL1 = TEMP1_GET_TEMP;
	masterSendTicket.VAL2 = RS_TEST_VAL2;
	masterSendTicket.VAL3 = RS_TEST_VAL3;

	if (FRTOS1_xTaskCreate(Task_RS_Send, "RSMaster", configMINIMAL_STACK_SIZE+100, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
		for(;;){} /* error */
	}
#endif
}

/*! \brief RS driver de-initialization */
void RS_Deinit(void){

}
