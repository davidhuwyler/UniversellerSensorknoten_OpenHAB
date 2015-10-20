/*
 * Platform.h
 *
 *  Created on: 21.03.2015
 *      Author: dave_sch_6
 */

#ifndef SOURCES_PLATFORM_H_
#define SOURCES_PLATFORM_H_

#include "PE_Types.h" /* for common Processor Expert types used throughout the project, e.g. 'bool' */
#include "PE_Error.h" /* global error constants */

void PL_Init(void);
void PL_Deinit(void);

#define PL_IS_MASTER 	(1)	/*Set to 1 if Sensor Node is used as Bus Master (only for Test applications)*/

#define PL_HAS_LED		(1)	/*Set to 1 for LED support otherwise zero*/

#define PL_HAS_NVM		(1)

#define PL_HAS_DEBOUNCE (1)


#define PL_NOF_PIR 		(2)	/*number of PIR Sensors connected*/
#define PL_NOF_KEYS		(2)	/*number of Switches connected*/
#define PL_NOF_LRD		(0)	/*number of Light Resistors connected*/
#define PL_NOF_REED 	(0)	/*number of Digital Reed Inputs connected*/
#define PL_NOF_RELAIS	(0)	/*number of Relais to */
#define PL_NOF_DHT		(0)	/*number of DHT Sensors connected*/
#define PL_HAS_STR		(0) /*Set to 1 if there is a LDR Stripe connected*/

#define PL_HAS_USB_CDC 	(1)

#define PL_HAS_OPENHAB	(1) /* Defines if Master uses Openhab */

#endif /* SOURCES_PLATFORM_H_ */
