//============================================================================
// Name        : KnapSack.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdint.h>
#include <stdio.h>

#include <iomanip>
#include <iostream>

using namespace std;

const uint16_t a = 93;
const uint16_t c = 14;
uint16_t current = 42069;

uint16_t pseudoRandInt() {
	current = (a * current + c);
	return current;
}

uint16_t max(uint64_t a, uint64_t b) {
	return a > b ? a : b;
}

int main() {

	cout << "Starting Analysis" << endl; // prints !!!Hello World!!!

	int enemyAmount, pokemonAmount, maxCost;
	int i, j, k;

	enemyAmount = 6;
	pokemonAmount = 1440;
	maxCost = 3500;

	double **battleResult = (double**) malloc(enemyAmount * sizeof(double*));
	for (i = 0; i < enemyAmount; i++) {
		battleResult[i] = (double*) malloc(pokemonAmount * sizeof(double*));
	}

	uint16_t *cost = (uint16_t*) malloc(pokemonAmount * sizeof(uint16_t*));

	for (i = 0; i < pokemonAmount; i++) {
		for (j = 0; j < enemyAmount; j++) {
			battleResult[j][i] = (pseudoRandInt() / (double) 0xFFFF);
		}
		cost[i] = pseudoRandInt() % 3500;

	}

	double ***DP = (double***) malloc(enemyAmount * sizeof(double**));
	for (i = 0; i < enemyAmount; i++) {
		DP[i] = (double**) malloc(pokemonAmount * sizeof(double*));
		for (j = 0; j < pokemonAmount; j++) {
			DP[i][j] = (double*) malloc((maxCost + 1) * sizeof(double));
		}
	}

	for (i = 0; i < enemyAmount; i++) {
		for (j = 0; j < pokemonAmount; j++) {
			for (k = 0; k <= maxCost; k++) {

				if (i == 0) { //Pokemon Boss 1
					if (cost[j] > k) { // Pokemon too expensive
						DP[i][j][k] = -1;
					} else if (j == 0 || DP[i][j - 1][k] == -1) { //No viable predecessor
						DP[i][j][k] = battleResult[i][j];
					} else {
						DP[i][j][k] = max(DP[i][j - 1][k], battleResult[i][j]);

					}

				} else { // Pokemon Boss 2..6

					if (DP[i - 1][j][k] == -1) { //Not possible at all for previous Boss
						DP[i][j][k] = -1;
					} else if (cost[j] > k) { //Pokemon too expensive
						DP[i][j][k] = -1;
					} else {
						int combinationBefore = k - cost[j];

						while (combinationBefore >= 0
								&& DP[i - 1][j][combinationBefore] == -1) {
							combinationBefore--;
						}

						if (combinationBefore == -1) {
							DP[i][j][k] = -1;
						} else {
							DP[i][j][k] = DP[i - 1][j][combinationBefore]
									+ battleResult[i][j];
						}
					}

					if (j != 0) { //Check predecessor
						DP[i][j][k] = max(DP[i][j - 1][k], DP[i][j][k]);
					}
				}
			}
		}

		for (int z = 0; z < pokemonAmount; z++) {
			cout << setfill('0') << setw(8) << DP[i][z][maxCost] << " ";
		}
		cout << endl;
	}

	int startPokemon = pokemonAmount - 1;
	int startCost = maxCost;
	double initialValue;

	for (int i = 5; i >= 0; i--) {

		initialValue = DP[i][startPokemon][startCost];

		while (startPokemon >= 0
				&& initialValue <= DP[i][startPokemon][startCost]) {
			startPokemon--;
		}
		startPokemon++;

		cout << "Pokemon " << setfill('0') << setw(4) << startPokemon
				<< " fights against Boss " << i + 1 << " and should win with "
				<< battleResult[i][startPokemon] << " hp " << endl;

		initialValue = initialValue - battleResult[i][startPokemon];
		startCost = startCost - cost[startPokemon];
	}

	cout << "Total HP Gained After fight is: "
			<< DP[enemyAmount - 1][pokemonAmount - 1][maxCost] << endl;

	return 0;
}
