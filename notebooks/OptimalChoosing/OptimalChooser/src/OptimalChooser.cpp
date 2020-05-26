#include <iostream>
#include <stdint.h>
#include <stdio.h>

#include <iomanip>
#include <iostream>
#include <chrono>

#include <fstream>
#include <sstream>

#include <unistd.h>

using namespace std;

// Values given:

// My PseudoRandom Stuff
const uint16_t a = 931;
const uint16_t c = 23;
uint16_t current = 12945;

// My Paths
string paths[5] = { "" };
int pathAmount = 5;

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

	/* DP[i][j][k] Most HP after fighting against the bosses 0...i
	 * using the j'th Pokemon to fight against the i'th boss at maximum cost k
	 * (Bosses 0...(i-1) fight against pokemons that have another race,
	 * apart from that we do not care about them) */

	/* Allocating Memory for DP Table */
	double ***DP = (double***) malloc(enemies * sizeof(double**));
	for (int i = 0; i < enemies; i++) {
		DP[i] = (double**) malloc(pokemons * sizeof(double*));
		for (int j = 0; j < pokemons; j++) {
			DP[i][j] = (double*) malloc((maxCost + 1) * sizeof(double));
		}
	}

	/* optimalChoice[i][j][k] stores the optimal hp value after a fight with bosses 0...i
	 * for the j'th pokemon race at k price
	 */

	/* Allocating Memory for optimalChoice */
	double ***optimalChoice = (double***) malloc(2 * sizeof(double**));
	for (int i = 0; i < 2; i++) {
		optimalChoice[i] = (double**) malloc((pokemons / 10) * sizeof(double*));
		for (int j = 0; j < pokemons / 10; j++) {
			optimalChoice[i][j] = (double*) malloc(
					(maxCost + 1) * sizeof(double));
		}
	}

	/* pokemonRaceUsed[i][j][k] stores the race of the pokemon that fights against the i'th pokemon
	 * at the optimal cost */
	int ***pokemonRaceUsed = (int***) malloc((enemies) * sizeof(int**));
	for (int i = 0; i < enemies; i++) {
		pokemonRaceUsed[i] = (int**) malloc((pokemons / 10) * sizeof(int*));
		for (int j = 0; j < pokemons / 10; j++) {
			pokemonRaceUsed[i][j] = (int*) malloc((maxCost + 1) * sizeof(int));
			for (int k = 0; k < maxCost + 1; k++) {
				pokemonRaceUsed[i][j][k] = -3;
			}
		}
	}

	uint8_t markArray[pokemons / 10] = { 0 };


	for (int i = 0; i < enemies; i++) {
		for (int j = 0; j < pokemons; j++) {
			for (int k = 0; k <= maxCost; k++) {

				if (i == 0) { //Pokemon Boss 0
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
						double maxHPBefore =
								optimalChoice[(i - 1) % 2][j / 10][priceBefore];

						if (maxHPBefore > -2) {
							DP[i][j][k] = maxHPBefore + battleResult[i][j];
						} else {
							DP[i][j][k] = -2;
						}
					}
				}

				int currentPokemon = j / 10;
				for (int x = i - 1; x >= 0; x--) {

					if (currentPokemon < 0 || k - cost[currentPokemon] < 0) {
						break;
					}

					markArray[currentPokemon] = 1;
					currentPokemon = pokemonRaceUsed[x][currentPokemon][k - cost[currentPokemon]];
				}

				for (int x = 0; x < pokemons / 10; x++) {
					if (markArray[x] == 0
							&& optimalChoice[i % 2][x][k] < DP[i][j][k]) {
						optimalChoice[i % 2][x][k] = DP[i][j][k];
						pokemonRaceUsed[i][x][k] = j / 10;
					}
				}

				for (int x = 0; x < pokemons / 10; x++) {
					markArray[x] = 0;
				}
			}
		}
	}

	return DP;
}

/* Backtracking the DP Table */
void backTrack(double ***DP, double **battleResults, uint16_t *cost,
		int enemies, int pokemons, int maxCost) {

	/* Find optimal result */
	int bestIndex = 0;
	for (int i = 0; i < pokemons; i++) {
		if (DP[enemies - 1][i][maxCost] > DP[enemies - 1][bestIndex][maxCost]) {
			bestIndex = i;
		}
	}

	/* We jump through the dp table, finding the values that the optimal value consists of*/
	int currentCost = maxCost;
	int currentIndex = bestIndex;
	double currentHP = DP[enemies - 1][bestIndex][maxCost];

	int totalCost = 0;

	for (int i = enemies; i >= 1; i--) {

		/* Find the pokemon that fights against the i'th boss */
		for (int j = 0; j < pokemons; j++) {
			if (currentHP - DP[i - 1][j][currentCost] < 0.0001) { //account for double imprecision
				currentIndex = j;
				break;
			}
		}

		totalCost += cost[currentIndex];

		cout << "Pokemon" << setfill(' ') << setw(5) << currentIndex
				<< " fights against Boss " << i << " and should win with "
				<< setfill(' ') << setw(10)
				<< battleResults[i - 1][currentIndex] << " hp with cost "
				<< cost[currentIndex] << endl;

		/* In order to jump through the DP Table we need to subtract the cost and HP of the current Pokemon*/
		currentCost -= cost[currentIndex];
		currentHP -= battleResults[i - 1][currentIndex];

	}

	cout << "Total HP: " << DP[enemies - 1][bestIndex][maxCost] << endl;
	cout << "Total Cost: " << totalCost << endl;

}

/*  ceates files/battleResults.csv with pseudo random values*/
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
			battleResults[j][i] = stod(tmp);
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
	int pokemonAmount = 150;
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
