/*
 * Sem.h
 *
 *  Created on: 18.04.2015
 *      Author: David Schwegler
 */

#ifndef SEM_H_
#define SEM_H_



bool SEM_takeRS_sem(void);
void SEM_give_RSsemFromISR(void);

/*! \brief Initialization of the module */
void SEM_Init(void);

/*! \brief Deinitialization of the module */
void SEM_Deinit(void);

#endif /* SEM_H_ */
