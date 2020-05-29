#include <iomanip>
#include <iostream>

#include "io_handler.h"

using namespace std;

double*** getDPTable(double **battleResult, int *cost, int enemies,
		int pokemons, int maxCost) {

	double ***DP = (double***) malloc(enemies * sizeof(double**));
	for (int i = 0; i < enemies; i++) {
		DP[i] = (double**) malloc((pokemons + 1) * sizeof(double*));
		for (int j = 0; j < pokemons + 1; j++) {
			DP[i][j] = (double*) malloc((maxCost + 1) * sizeof(double));
		}
	}

	double ***optimalChoice = (double***) malloc(enemies * sizeof(double**));
	for (int i = 0; i < enemies; i++) {
		optimalChoice[i] = (double**) malloc((pokemons / 10) * sizeof(double*));
		for (int j = 0; j < pokemons / 10; j++) {
			optimalChoice[i][j] = (double*) malloc(
					(maxCost + 1) * sizeof(double));
			for (int k = 0; k <= maxCost; k++) {
				optimalChoice[i][j][k] = -40;
			}
		}
	}

	double ***usedPokemons = (double***) malloc(enemies * sizeof(double**));
	for (int i = 0; i < enemies; i++) {
		usedPokemons[i] = (double**) malloc((pokemons / 10) * sizeof(double*));
		for (int j = 0; j < pokemons / 10; j++) {
			usedPokemons[i][j] = (double*) malloc(
					(maxCost + 1) * sizeof(double));
			for (int k = 0; k <= maxCost; k++) {
				usedPokemons[i][j][k] = -1;
			}
		}
	}

	int ignorePokemons[enemies] = { 0 };
	for (int i = 0; i < enemies; i++) {
		ignorePokemons[i] = -1;
	}

	int races[pokemons / 10] = { 0 };

	for (int i = 0; i < enemies; i++) {
		for (int j = 0; j < pokemons; j++) {
			for (int k = 0; k <= maxCost; k++) {

				for (int x = 0; x < enemies; x++) {
					if (ignorePokemons[x] != -1) {
						races[ignorePokemons[x]] = 0;
					}
				}

				for (int x = 0; x < enemies; x++) {
					ignorePokemons[x] = -1;
				}

				ignorePokemons[i] = j / 10;

				if (cost[j] > k) {
					DP[i][j][k] = -40;
				} else if (i == 0) {
					DP[i][j][k] = battleResult[i][j];
				} else if (optimalChoice[i - 1][j / 10][k - cost[j]] == -40) {
					DP[i][j][k] = -40;
				} else {
					DP[i][j][k] = battleResult[i][j]
							+ optimalChoice[i - 1][j / 10][k - cost[j]];

					int currentCost = k - cost[j];
					int currentPokemon = j;

					for (int x = i - 1; x >= 0; x--) {
						currentPokemon =
								usedPokemons[x][currentPokemon / 10][currentCost];
						currentCost = currentCost - cost[currentPokemon];
						ignorePokemons[x] = currentPokemon / 10;
					}
				}

				for (int x = 0; x < enemies; x++) {
					if (ignorePokemons[x] != -1) {
						races[ignorePokemons[x]] = 1;
					}
				}

				for (int x = 0; x < pokemons / 10; x++) {
					if (races[x] == 0) {
						if (optimalChoice[i][x][k] < DP[i][j][k]) {
							optimalChoice[i][x][k] = DP[i][j][k];
							usedPokemons[i][x][k] = j;
						}
					}
				}
			}
		}
	}

	return DP;
}

/* Backtracking the DP Table */
void backTrack(double ***DP, double **battleResults, int *cost, int enemies,
		int pokemons, int maxCost) {

	double totalHP = 0;

	int totalCost = 0;
	int currentPokemon = 0;
	int currentCost = maxCost;

	int submissionData[enemies] = { 0 };
	int races[pokemons / 10] = { 0 };

	for (int i = enemies - 1; i >= 0; i--) {

		currentPokemon = -1;
		for (int j = 0; j < pokemons; j++) {

			if (races[j / 10] == 0) { // Not allowed to chose a pokemon of the same race twice
				if (currentPokemon < 0) { // We haven't set maxIndex yet
					currentPokemon = j;
				} else if (DP[i][currentPokemon][currentCost]
						< DP[i][j][currentCost]) { // There is a pokemon with a better result
					currentPokemon = j;
				} else if (DP[i][currentPokemon][currentCost]
						== DP[i][j][currentCost]
						&& cost[currentPokemon] > cost[j]) { // There is a pokemon with the same result at a lower cost
					currentPokemon = j;
				}
			}
		}

		races[currentPokemon / 10] = 1; //Mark pokemon that was used

		totalHP += battleResults[i][currentPokemon] - 1;
		totalCost += cost[currentPokemon];
		submissionData[i] = currentPokemon;

		currentCost -= cost[currentPokemon];

		cout << "Pokemon" << setfill(' ') << setw(5) << currentPokemon
				<< " fights against Boss " << i << " and should have "
				<< setfill(' ') << setw(10)
				<< battleResults[i][currentPokemon] - 1 << " hp and cost "
				<< cost[currentPokemon] << endl;

	}

	cout << "Total HP Gained After fight is: " << totalHP << " at cost "
			<< totalCost << endl;
	cout << endl;

	createSubmission((int*) submissionData, enemies);
}
