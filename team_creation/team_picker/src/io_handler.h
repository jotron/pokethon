/*
 * IOHandler.h
 *
 *  Created on: May 29, 2020
 *      Author: rahul
 */

#ifndef IO_HANDLER_H_
#define IO_HANDLER_H_

double** getBattleResults(int enemyAmount, int pokemonAmount);

int* getCost(int pokemonAmount);

void createSubmission(int *submissionData, int enemies);

void setPath();

#endif /* IO_HANDLER_H_ */
