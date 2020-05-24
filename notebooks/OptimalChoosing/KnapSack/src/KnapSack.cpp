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

	int optimalChoice[enemies][maxCost] = { { -1 } };
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
						DP[i][j][k] = -2;
					} else {
						DP[i][j][k] = battleResult[i][j];
					}

				} else { // Pokemon Boss 2..6

					if (cost[j] > k) { //Pokemon too expensive
						DP[i][j][k] = -1;
					} else {
						int priceBefore = k - cost[j];
						double maxHPBefore = DP[i -1][optimalChoice[i - 1][priceBefore]][priceBefore];

						if (maxHPBefore > -2) {
							DP[i][j][k] = maxHPBefore + battleResult[i][j];
						} else {
							DP[i][j][k] = -2;
						}
					}
				}

				if (optimalChoice[i][k] == -1 || DP[i][optimalChoice[i][k]][k] < DP[i][j][k]) {
					optimalChoice[i][k] = j;
				}

			}

		}
	}

	return DP;
}

/* Backtracking the DP Table */
void backTrack(double ***DP, double **battleResults, uint16_t *cost,
		int enemies, int pokemons, int maxCost) {

	int bestIndex = 0;
	for (int i = 0; i < pokemons; i++) {
		if (DP[enemies - 1][i][maxCost] > DP[enemies - 1][bestIndex][maxCost]) {
			bestIndex = i;
		}
	}

	int currentCost = maxCost;
	int currentIndex = bestIndex;
	double currentHP = DP[enemies - 1][bestIndex][maxCost];

	int totalCost = cost[bestIndex];
	cout << currentIndex << " " << cost[currentIndex] << " "
			<< battleResults[enemies - 1][currentIndex] << endl;

	for (int i = enemies - 1; i >= 1; i--) {

		currentCost -= cost[currentIndex];
		currentHP -= battleResults[i][currentIndex];

		for (int j = 0; j < pokemons; j++) {
			//cout << currentHP << " " << DP[i - 1][j][currentCost] << endl;
			if (currentHP - DP[i - 1][j][currentCost] < 0.001) {
				currentIndex = j;
				//cout << "Found" << endl;
				break;
			}
		}

		cout << currentIndex << " " << cost[currentIndex] << " "
				<< battleResults[i - 1][currentIndex] << endl;
		totalCost += cost[currentIndex];

	}
	cout << "Total HP Gained After fight is: "
			<< DP[enemies - 1][bestIndex][maxCost] << " at cost " << totalCost
			<< endl;

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

	getline(inputFile, line);
	for (int i = 0; i < pokemonAmount; i++) {
		getline(inputFile, line);
		stringstream s(line);
		getline(s, tmp, ','); //Remove ID
		getline(s, tmp, ','); //Remove ID
		for (int j = 0; j < enemyAmount; j++) {
			getline(s, tmp, ',');
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

	for (int i = 0; i < pokemonAmount; i++) {
		getline(inputFile, line);
		cost[i] = (uint16_t) stoi(line);
	}

	inputFile.close();

	return cost;
}

int main() {

	int enemyAmount = 6;
	int pokemonAmount = 1440;
	int maxCost = 3500;

	cout << "Starting Analysis" << endl; // prints !!!Hello World!!!

	//freopen("out.txt","w",stdout);
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
