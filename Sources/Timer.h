/*
 * Timer.h
 *
 *  Created on: 24.03.2015
 *      Author: dave_sch_6
 */

#ifndef SOURCES_TIMER_H_
#define SOURCES_TIMER_H_

#define TMR_TICK_MS  1
  /*!< we get called every TMR_TICK_MS ms */

/*! \brief Function called from timer interrupt every TMR_TICK_MS. */
void TMR_OnInterrupt(void);

/*! \brief Timer driver initialization */
void TMR_Init(void);

/*! \brief Timer driver de-initialization */
void TMR_Deinit(void);

#endif /* SOURCES_TIMER_H_ */
