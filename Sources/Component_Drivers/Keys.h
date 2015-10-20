/*
 * Keys.h
 *
 *  Created on: 08.04.2015
 *      Author: David Schwegler
 */

#ifndef KEYS_H_
#define KEYS_H_

#include "Platform.h"

typedef enum {
#if PL_NOF_KEYS>=1
  KEY_BTN1, /*!< button 1 */
#endif
#if PL_NOF_KEYS>=2
  KEY_BTN2, /*!< button 2 */
#endif
  KEY_BTN_LAST /*!< must be last */
} KEY_Buttons;

#if PL_NOF_KEYS>=1
  #include "SW1.h"

  #define KEY1_Get()  (!(SW1_GetVal()))
    /*!< Macro which returns TRUE if key is pressed */
#else
  #define KEY1_Get()  FALSE
    /*!< if we do not have a button, then return 'not pressed' */
#endif

#if PL_NOF_KEYS>=2
  #include "SW2.h"

  #define KEY2_Get()  (!(SW2_GetVal()))
    /*!< Macro which returns TRUE if key is pressed */
#else
  #define KEY2_Get()  FALSE
    /*!< if we do not have a button, then return 'not pressed' */
#endif

/*!
 * \brief Function called from a keyboard interrupt (if supported).
 * \param button Button for which interrupt has been generated.
 */
void KEY_OnInterrupt(KEY_Buttons button);

/*!
 * \brief Acknowledge any pending interrupt, and enable again keyboard interrupts
 */
void KEY_EnableInterrupts(void);

/*!
 * \brief Acknowledge any pending interrupt, and enable again keyboard interrupts
 */
void KEY_DisableInterrupts(void);

/*!
 * \brief Checks the key status and generates the events.
 */
void KEY_Scan(void);

/*! \brief Get access to key hardware. Use KEY_Close() afterwards. */
void KEY_Open(void);

/*! \brief Finish access to key hardware. Called after KEY_Open(). */
void KEY_Close(void);

/*! \brief Key driver initialization */
void KEY_Init(void);

/*! \brief Key driver de-initialization */
void KEY_Deinit(void);


#endif /* KEYS_H_ */
