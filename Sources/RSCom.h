/*
 * RSCom.h
 *
 *  Created on: 10.04.2015
 *      Author: David Schwegler
 */

#ifndef RSCOM_H_
#define RSCOM_H_

#include "Platform.h"
#include "AS1.h"
#include "CLS1.h"

#define NOF_BYTES_IN_TICKET (17)	/* Define the size of an incoming Ticket*/

/*! \brief RS Communication Module initialization, creates Read/Write Task task */
void RS_Init(void);

/*! \brief RS driver de-initialization */
void RS_Deinit(void);

/*!
 * \brief Sends a Byte over AS1 on RS485 Bus
 * \param msg Zero terminated string to write
 */
void RS_SendByte(AS1_TComData msg);


uint8_t RS_ParseCommand(const uint8_t *cmd, bool *handled, CLS1_ConstStdIOType *io);


#endif /* RSCOM_H_ */
