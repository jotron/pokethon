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
#include <chrono>

#include <fstream>
#include <sstream>

using namespace std;

// Values given:

// My PseudoRandom Stuff
const uint16_t a = 931;
const uint16_t c = 23;
uint16_t current = 12945;

uint16_t pseudoRandInt() {
	current = (a * current + c);
	return current;
}

// Helper Functions
uint16_t max(uint16_t a, uint16_t b) {
	return a > b ? a : b;
}

//DP Algorithm
double*** getDPTable(double **battleResult, uint16_t *cost, int enemies,
		int pokemons, int maxCost) {

	double ***DP = (double***) malloc(enemies * sizeof(double**));
	for (int i = 0; i < enemies; i++) {
		DP[i] = (double**) malloc(pokemons * sizeof(double*));
		for (int j = 0; j < pokemons; j++) {
			DP[i][j] = (double*) malloc((maxCost + 1) * sizeof(double));
		}
	}

	for (int i = 0; i < enemies; i++) {
		for (int j = 0; j < pokemons; j++) {
			for (int k = 0; k <= maxCost; k++) {

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
						int priceBefore = k - cost[j];
						int pokemonsBefore = j - (j % 10) - 1; // Not allowed to use the same pokemon Name more than once
						int enemiesBefore = i - 1;

						while (pokemonsBefore >= 0 && priceBefore >= 0
								&& DP[enemiesBefore][pokemonsBefore][priceBefore] == -1) {
							priceBefore--;
						}

						if (priceBefore < -1 || pokemonsBefore < 0) {
							DP[i][j][k] = -1;
						} else {
							DP[i][j][k] = DP[enemiesBefore][pokemonsBefore][priceBefore]
									+ battleResult[i][j];
						}
					}

					if (j != 0) { //Check predecessor
						DP[i][j][k] = max(DP[i][j - 1][k], DP[i][j][k]);
					}
				}
			}
		}
	}

	return DP;
}

/* Backtracking the DP Table */
void backTrack(double ***DP, double **battleResult, uint16_t *cost, int enemies,
		int pokemons, int maxCost) {

	int startPokemon = pokemons - 1;
	int startCost = maxCost;
	double initialValue;

	int totalCost = 0;
	for (int i = enemies - 1; i >= 0; i--) {

		initialValue = DP[i][startPokemon][startCost];

		startPokemon = startPokemon - (startPokemon % 10) - 1;
		while (startPokemon >= 0
				&& initialValue <= DP[i][startPokemon][startCost]) {
			startPokemon--;
		}
		startPokemon++;

		cout << "Pokemon " << setfill(' ') << setw(4) << startPokemon
				<< " fights against Boss " << i + 1 << " and should win with "
				<< setfill(' ') << setw(8) << battleResult[i][startPokemon]
				<< " hp and cost " << cost[startPokemon] << endl;

		initialValue = initialValue - battleResult[i][startPokemon];
		startCost = startCost - cost[startPokemon];

		totalCost += cost[startPokemon];
	}

	cout << "Total HP Gained After fight is: "
			<< DP[enemies - 1][pokemons - 1][maxCost] << " at cost "
			<< totalCost << endl;

}

/* ceates files/battleResults.csv with pseudo random values*/
void createBattleResults(int enemyAmount, int pokemonAmount) {

	string inputFileName = "files/battleResults.csv";
	ofstream outputFile;
	outputFile.open(inputFileName);

	// Write random values into the rows
	for (int i = 0; i < pokemonAmount; i++) {
		for (int j = 0; j < enemyAmount; j++) {
			double value = (pseudoRandInt() / (double) 0xFFFF);
			outputFile << value << ";";
		}
		outputFile << endl;
	}
	outputFile.close();

}

/* ceates files/cost.csv with pseudo random values*/
void createCost(int pokemonAmount) {
	string inputFileName = "files/cost.csv";
	ofstream outputFile;
	outputFile.open(inputFileName);

	// Write random values into the rows
	for (int i = 0; i < pokemonAmount; i++) {
		outputFile << pseudoRandInt() % 3500 << ";";
	}

	outputFile << endl;
	outputFile.close();
}

/* Reads Battle results from "files/battleResults.csv" */
double** getBattleResults(int enemyAmount, int pokemonAmount) {

	double **battleResults = (double**) malloc(enemyAmount * sizeof(double*));
	for (int i = 0; i < enemyAmount; i++) {
		battleResults[i] = (double*) malloc(pokemonAmount * sizeof(double*));
	}

	string inputFileName = "files/battleResults.csv";
	ifstream inputFile;

	inputFile.open(inputFileName);
	if (!inputFile.is_open()) { /* validate file open for reading */
		cerr << "error: No file given " << inputFileName << "'.\n";
		createBattleResults(enemyAmount, pokemonAmount);
	}

	string line;
	string tmp;

	for (int i = 0; i < pokemonAmount; i++) {
		getline(inputFile, line);
		stringstream s(line);
		for (int j = 0; j < enemyAmount; j++) {
			getline(s, tmp, ';');
			battleResults[j][i] = stod(tmp);
		}
	}

	inputFile.close();

	return battleResults;
}

/* Reads Battle results from "files/cost.csv" */
uint16_t* getCost(int pokemonAmount) {
	string inputFileName = "files/cost.csv";
	ifstream inputFile;

	uint16_t *cost = (uint16_t*) malloc(pokemonAmount * sizeof(uint16_t*));

	inputFile.open(inputFileName);
	if (!inputFile.is_open()) { /* validate file open for reading */
		cerr << "error: No file given " << inputFileName << "'.\n";
		return 0;
	}

	string line;
	string tmp;
	getline(inputFile, line);
	stringstream s(line);

	for (int i = 0; i < pokemonAmount; i++) {
		getline(s, tmp, ';');
		cost[i] = (uint16_t) stoi(tmp);
	}

	inputFile.close();

	return cost;
}

int main() {

	int enemyAmount = 6;
	int pokemonAmount = 1440;
	int maxCost = 3500;

	createBattleResults(enemyAmount, pokemonAmount);
	createCost(pokemonAmount);

	cout << "Starting Analysis" << endl; // prints !!!Hello World!!!

	/* Input */
	double **battleResults = getBattleResults(enemyAmount, pokemonAmount);
	uint16_t *cost = getCost(pokemonAmount);

	/* Time */
	auto start = chrono::high_resolution_clock::now();

	/* DP algorithm */
	double ***DP = getDPTable(battleResults, cost, enemyAmount, pokemonAmount,
			maxCost);
	backTrack(DP, battleResults, cost, enemyAmount, pokemonAmount, maxCost);

	/* Time */
	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = finish - start;
	cout << "Elapsed time: " << elapsed.count() << " seconds\n";

	return 0;
}
