/*
 * Keys.c
 *
 *  Created on: 08.04.2015
 *      Author: David Schwegler
 */

/**
 *
 * This module implements a generic keyboard driver for up to 7 Keys.
 * It is using macros for maximum flexibility with minimal code overhead.
 */

#include "Platform.h"
#include "Event.h"
#include "Keys.h"
#include "KeyDebounce.h"


void KEY_Scan(void) {
	/* used if there is a polled Sensor*/
}

void KEY_OnInterrupt(KEY_Buttons button) {
#if PL_HAS_DEBOUNCE
  KEYDBNC_Process();
#else
  switch(button) {
#if PL_NOF_KEYS >= 1
    case KEY_BTN1: EVENT_SetEvent(EVENT_SW1_PRESSED); break;
#endif
#if PL_NOF_KEYS >= 2
    case KEY_BTN2: EVENT_SetEvent(EVENT_SW2_PRESSED); break;
#endif
    default:
      /* unknown? */
      break;
  } /* switch */
#endif
}

void KEY_EnableInterrupts(void) {
#if PL_NOF_KEYS >= 1
  SW1_Enable();
#endif
#if PL_NOF_KEYS >= 2
  SW2_Enable();
#endif
}

void KEY_DisableInterrupts(void) {
#if PL_NOF_KEYS >= 1
  SW1_Disable();
#endif
#if PL_NOF_KEYS >= 2
  SW2_Disable();
#endif
}

void PORTA_OnInterrupt(void) {
  void Cpu_ivINT_PORTA(void); /* prototype of ISR in Cpu.c */

  Cpu_ivINT_PORTA(); /* call interrupt handler created by the ExtInt components */
}

/*! \brief Key driver initialization */
void KEY_Init(void) {
  /* nothing needed for now */
}

/*! \brief Key driver de-initialization */
void KEY_Deinit(void) {
  /* nothing needed for now */
}



