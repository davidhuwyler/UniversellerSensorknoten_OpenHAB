/**
 * \file
 * \brief RTOS interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module the main tasks of the RTOS.
 */

#ifndef RTOS_H_
#define RTOS_H_

/*! \brief Runs the scheduler */
void RTOS_Run(void);

/*! \brief Initializes the RTOS module */
void RTOS_Init(void);

/*! \brief De-Initializes the RTOS module */
void RTOS_Deinit(void);

#endif /* RTOS_H_ */
