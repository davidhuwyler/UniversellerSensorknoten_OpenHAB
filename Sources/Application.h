/*
 * Application.h
 *
 *  Created on: 21.03.2015
 *      Author: dave_sch_6
 */


/*!
** @file Application.h
** @version 01.00
** @author David Schwegler, david.schwegler@stud.hslu.ch
** @brief
**  	Application main routine.
**      This module is our main loop program so you don't have to modify main.c.
**      This module manage all events.
** @detail
**
*/


#ifndef SOURCES_APPLICATION_H_
#define SOURCES_APPLICATION_H_

/*!
 * \brief Application initialization
 */
void initApplication();

/*!
 * \brief Application main loop
 *
 */
void runApplication();

/*!
 * \brief Application deinitialization. Note that this code normally never is executed!
 *
 */
void deinitApplication();

void runTask();


#endif /* SOURCES_APPLICATION_H_ */
