#include <iomanip>
#include <iostream>

#include "io_handler.h"

using namespace std;

/* DP[i][j][k] = The maximum amount of HP after fighting pokemon champions 0...i
 *  under the condition that pokemon j fights against pokemon i
 *  and the sum of the costs of the pokemons who were chosen to fight is maximum k
 *  If this is not possible at all, we chose DP[i][j][k] = -40*/

/* optimalChoice[i][j][k] = The maximum amount of HP after fighting pokemon champions 0...i
 * under the condition that no pokemon with name j is used to fight these pokemons
 * and the sum of the costs of the pokemons who were chosen to fight is maximum k  */

/* usedPokemons[i][j][k] = The pokemon that fights i in optimalChoice[i][j][k] */

double*** getDPTable(double **battleResult, int *cost, int champions,
		int pokemons, int maxCost) {

	/* Allocating to heap, because it is large */
	double ***DP = (double***) malloc(champions * sizeof(double**));
	for (int i = 0; i < champions; i++) {
		DP[i] = (double**) malloc((pokemons + 1) * sizeof(double*));
		for (int j = 0; j < pokemons + 1; j++) {
			DP[i][j] = (double*) malloc((maxCost + 1) * sizeof(double));
		}
	}

	/* Allocating to heap, because it is large */
	double ***optimalChoice = (double***) malloc(champions * sizeof(double**));
	for (int i = 0; i < champions; i++) {
		optimalChoice[i] = (double**) malloc((pokemons / 10) * sizeof(double*));
		for (int j = 0; j < pokemons / 10; j++) {
			optimalChoice[i][j] = (double*) malloc(
					(maxCost + 1) * sizeof(double));
			for (int k = 0; k <= maxCost; k++) {
				optimalChoice[i][j][k] = -40;
			}
		}
	}

	/* Allocating to heap, because it is large */
	double ***usedPokemons = (double***) malloc(champions * sizeof(double**));
	for (int i = 0; i < champions; i++) {
		usedPokemons[i] = (double**) malloc((pokemons / 10) * sizeof(double*));
		for (int j = 0; j < pokemons / 10; j++) {
			usedPokemons[i][j] = (double*) malloc(
					(maxCost + 1) * sizeof(double));
			for (int k = 0; k <= maxCost; k++) {
				usedPokemons[i][j][k] = -1;
			}
		}
	}

	/* We store the pokemon names that were used to fight in this array */
	int chosenPokemons[champions] = { 0 };
	for (int i = 0; i < champions; i++) {
		chosenPokemons[i] = -1; //We chose no pokemons in the beginning
	}

	/* We store which pokemons names we should ignore in this array
	 * while updating optimalChoice and usedPokemons */
	int ignoreNames[pokemons / 10] = { 0 };

	for (int i = 0; i < champions; i++) {
		for (int j = 0; j < pokemons; j++) {
			for (int k = 0; k <= maxCost; k++) {

				/* From the definition of the DP table, we always choose the j'th pokemon */
				chosenPokemons[i] = j / 10;

				if (cost[j] > k) { //Pokemon is too expensive to be used to fight i at price k
					DP[i][j][k] = -40;
				} else if (i == 0) { //Only one champion, so no previous fights to worry about
					DP[i][j][k] = battleResult[i][j];
				} else if (optimalChoice[i - 1][j / 10][k - cost[j]] == -40) { //Impossible to fight pokemon 0...i at price k
					DP[i][j][k] = -40;
				} else { //Multiple champions to fight,
					DP[i][j][k] = battleResult[i][j]
							+ optimalChoice[i - 1][j / 10][k - cost[j]]; //chose optimalChoice for current pokemon name

					/* Determine the pokemons that were used to form optimalChoice[i - 1][j / 10][k - cost[j]] */
					int currentCost = k - cost[j];
					int currentPokemon = j;

					/* We find out which pokemon was used to fight boss i-1, i-2, .. , 0 by backtracking the usedPokemons */
					for (int x = i - 1; x >= 0; x--) {
						currentPokemon =
								usedPokemons[x][currentPokemon / 10][currentCost];
						currentCost = currentCost - cost[currentPokemon];
						chosenPokemons[x] = currentPokemon / 10;
					}
				}

				/* See what pokemons we chose to fight */
				for (int x = 0; x < champions; x++) {
					if (chosenPokemons[x] != -1) {
						ignoreNames[chosenPokemons[x]] = 1;
					}
				}

				/* Update optimalChoice of every pokemon name, expect the ones that were used to fight */
				for (int x = 0; x < pokemons / 10; x++) {
					if (ignoreNames[x] == 0) {
						if (optimalChoice[i][x][k] < DP[i][j][k]) {
							optimalChoice[i][x][k] = DP[i][j][k];
							usedPokemons[i][x][k] = j;
						}
					} else {
						ignoreNames[x] = 0; //We need to reset this array for the next iteration
						// We don't need to do this for chosenPokemons, because we overwrite every value in every iteration
					}
				}
			}
		}
	}

	return DP;
}

/* Backtracking the DP Table */
int* backTrack(double ***DP, double **battleResults, int *cost, int champions,
		int pokemons, int maxCost, int *submissionData) {

	double totalHP = 0;

	int totalCost = 0;
	int currentPokemon = 0;
	int currentCost = maxCost;

	int names[pokemons / 10] = { 0 };

	for (int i = champions - 1; i >= 0; i--) {

		currentPokemon = -1;
		for (int j = 0; j < pokemons; j++) {

			if (names[j / 10] == 0) { // Not allowed to chose a pokemon name twice
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

		names[currentPokemon / 10] = 1; //Mark pokemon that was used
		currentCost -= cost[currentPokemon]; //Subtract cost of current Pokemon

		cout << "Pokemon" << setfill(' ') << setw(5) << currentPokemon + 1
				<< " fights against Champion " << i + 1 << " and should have "
				<< setfill(' ') << setw(10) << battleResults[i][currentPokemon]
				<< " hp and cost " << cost[currentPokemon] << endl;

		/* Not necessary for backtracking, just for output */
		totalHP += battleResults[i][currentPokemon];
		totalCost += cost[currentPokemon];
		submissionData[i] = currentPokemon + 1;


	}

	cout << "Total HP Remaining after fighting is: " << totalHP << " at cost "
			<< totalCost << endl;
	cout << endl;

	return submissionData;
}
