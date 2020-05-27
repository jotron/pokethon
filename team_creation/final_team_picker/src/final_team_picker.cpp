#include <iostream>
#include <stdint.h>
#include <stdio.h>

#include <iomanip>
#include <iostream>
#include <chrono>

#include <fstream>
#include <sstream>
#include <math.h>

#include <unistd.h>

using namespace std;

// My Paths
string paths[5] = { "" };
int pathAmount = 5;

//DP Algorithm
double*** getDPTable(double **battleResult, uint16_t *cost, int enemies,
		int pokemons, int maxCost) {

	double ***DP = (double***) malloc(enemies * sizeof(double**));
	for (int i = 0; i < enemies; i++) {
		DP[i] = (double**) malloc((pokemons + 1) * sizeof(double*));
		for (int j = 0; j < pokemons + 1; j++) {
			DP[i][j] = (double*) malloc((maxCost + 1) * sizeof(double));
		}
	}

	for (int i = 0; i < enemies; i++) {
		for (int j = 0; j <= pokemons; j++) {

			for (int k = 0; k <= maxCost; k++) {

				if (j == 0 || k == 0) {
					DP[i][j][k] = -40;
				} else if (cost[j - 1] > k) {
					DP[i][j][k] = -40;
				} else if (i == 0) {
					DP[i][j][k] = battleResult[i][j - 1];
				} else if (DP[i][pokemons][k - cost[j - 1]] == -40) {
					DP[i][j][k] = -40;
				} else {
					DP[i][j][k] = battleResult[i][j - 1]
							+ DP[i - 1][pokemons][k - cost[j - 1]];
				}

				if (j > 0) {
					DP[i][j][k] = max(DP[i][j][k], DP[i][j - 1][k]);
				}
			}
		}
	}

	return DP;
}

/* Backtracking the DP Table */
void backTrack(double ***DP, double **battleResults, uint16_t *cost,
		int enemies, int pokemons, int maxCost) {

	int currentCost = maxCost;
	int totalCost = 0;

	for (int i = enemies - 1; i >= 0; i--) {

		for (int j = pokemons - 2; j >= 0; j--) {
			if (DP[i][pokemons][currentCost] != DP[i][j][currentCost]) {

				currentCost -= cost[j];
				totalCost += cost[j];

				cout << "Pokemon" << setfill(' ') << setw(5) << j
								<< " fights against Boss " << i << " and should have "
								<< setfill(' ') << setw(10)
								<< battleResults[i][j] - 1 << " hp and cost "
								<< cost[j] << endl;
				break;
			}
		}

	}


	cout << "Total HP Gained After fight is: "
			<< DP[enemies - 1][pokemons][maxCost] - 6 << " at cost " << totalCost
			<< endl;

}

ifstream openInputFile(string inputFileName) {

	ifstream inputFile;

	/* Look at all paths given */
	int i = pathAmount - 1;
	do {
		inputFile.close();
		inputFile.open(paths[i] + inputFileName);
		i--;

	} while (!inputFile.is_open() && i >= 0);

	if (i == -1) {
		cerr << "error: No file found " << inputFileName << "'.\n";
		exit(-1);
	}

	cout << "reading " << paths[i] + inputFileName << endl;

	return inputFile;
}

/* Reads Battle results from "files/battleResults.csv" */
double** getBattleResults(int enemyAmount, int pokemonAmount) {

	/* Allocate array for storing battleResults */
	double **battleResults = (double**) malloc(enemyAmount * sizeof(double*));
	for (int i = 0; i < enemyAmount; i++) {
		battleResults[i] = (double*) malloc(pokemonAmount * sizeof(double*));
	}

	ifstream inputFile = openInputFile("/battleResults.csv");

	string line; //the i'th line contains the battle results of the i'th pokemon
	string tmp;

	getline(inputFile, line); //Header not necessary
	for (int i = 0; i < pokemonAmount; i++) {
		getline(inputFile, line);
		stringstream s(line);
		getline(s, tmp, ','); //Column not necessary
		getline(s, tmp, ','); //Column not necessary
		for (int j = 0; j < enemyAmount; j++) {
			getline(s, tmp, ',');
			battleResults[j][i] = stod(tmp) + 1;
		}
	}

	inputFile.close();

	return battleResults;
}

/* Reads Battle results from "cost.csv" */
uint16_t* getCost(int pokemonAmount) {

	/* Allocate array for storing cost */
	uint16_t *cost = (uint16_t*) malloc(pokemonAmount * sizeof(uint16_t*));

	ifstream inputFile = openInputFile("/cost.csv");

	string line; //line i has i'th pokemon cost
	for (int i = 0; i < pokemonAmount; i++) {
		getline(inputFile, line);
		cost[i] = (uint16_t) stoi(line);
	}

	inputFile.close();

	return cost;
}

int main(int argc, char **argv) {

	/* Manage all possible paths of input files */
	char temp[10000];
	paths[0] = getcwd(temp, sizeof(temp));
	paths[1] = paths[0].substr(0, paths[0].find_last_of("/\\"));
	paths[2] = paths[0] + "/files";
	paths[3] = paths[1] + "/files";

	if (argc > 1) {
		paths[5] = argv[1];
		cout << "Path given " << argv[1] << endl;
	}

	/* Set values for reading files and anlyzing data*/
	int enemyAmount = 6;
	int pokemonAmount = 1440;
	int maxCost = 3500;

	/* Input */
	cout << "Obtaining Data " << endl;

	auto start = chrono::high_resolution_clock::now(); //time measurement

	double **battleResults = getBattleResults(enemyAmount, pokemonAmount);
	uint16_t *cost = getCost(pokemonAmount);

	auto finish = chrono::high_resolution_clock::now(); //time measurement
	chrono::duration<double> elapsed = finish - start; //time measurement

	cout << "Time needed for obtaining data: " << elapsed.count()
			<< " seconds\n" << endl;

	/* Analysis */
	cout << "Analyzing Data " << endl;

	start = chrono::high_resolution_clock::now(); //time measurement

	/* DP algorithm */
	double ***DP = getDPTable(battleResults, cost, enemyAmount, pokemonAmount,
			maxCost);
	backTrack(DP, battleResults, cost, enemyAmount, pokemonAmount, maxCost);

	finish = chrono::high_resolution_clock::now(); //time measurement
	elapsed = finish - start; //time measurement

	cout << "Time needed for analyzing data: " << elapsed.count()
			<< " seconds\n";

	return 0;
}
