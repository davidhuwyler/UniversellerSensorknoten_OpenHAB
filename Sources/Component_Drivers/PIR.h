/*
 * PIR.h
 *
 *  Created on: 07.04.2015
 *      Author: David Schwegler
 */

#ifndef PIR_H_
#define PIR_H_

#include "Platform.h"

typedef enum {
#if PL_NOF_PIR>=1
  KEY_PIR1, /*!< Motion Sensor 1 */
#endif
#if PL_NOF_PIR>=2
  KEY_PIR2, /*!< Motion Sensor 2 */
#endif

  KEY_PIR_LAST /*!< must be last */
} KEY_PIR;

#if PL_NOF_PIR>=1
  #include "PIR1.h"

  #define PIR1_Get()  (!(PIR1_GetVal()))
    /*!< Macro which returns TRUE if key is pressed */
#else
  #define PIR1_Get()  FALSE
    /*!< if we do not have a button, then return 'not pressed' */
#endif

#if PL_NOF_PIR>=2
  #include "PIR2.h"

  #define PIR2_Get()  (!(PIR2_GetVal()))
    /*!< Macro which returns TRUE if key is pressed */
#else
  #define PIR2_Get()  FALSE
    /*!< if we do not have a button, then return 'not pressed' */
#endif

/*!
 * \brief Function called from a Sensor interrupt (if supported).
 * \param PIR_sens Motion Sensor for which interrupt has been generated.
 */
void PIR_OnInterrupt(KEY_PIR PIR_sens);

/*!
 * \brief Acknowledge any pending interrupt, and enable again keyboard interrupts
 */
void PIR_EnableInterrupts(void);

/*!
 * \brief Acknowledge any pending interrupt, and disable keyboard interrupts
 */
void PIR_DisableInterrupts(void);

/*!
 * \brief Checks the PIR status and generates the events.
 */
void PIR_Scan(void);

/*! \brief Get access to sensor hardware. Use PIR_Close() afterwards. */
void PIR_Open(void);

/*! \brief Finish access to Sensor hardware. Called after PIR_Open(). */
void PIR_Close(void);

/*! \brief PIR driver initialization */
void PIR_Init(void);

/*! \brief PIR driver de-initialization */
void PIR_Deinit(void);


#endif /* PIR_H_ */
