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
string path = "";
int pathAmount = 5;

//DP Algorithm
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

	for (int i = 0; i < enemies; i++) {
		for (int j = 0; j < pokemons; j++) {
			for (int k = 0; k <= maxCost; k++) {

				if (cost[j] > k) {
					DP[i][j][k] = -40;
				} else if (i == 0) {
					DP[i][j][k] = battleResult[i][j];
				} else if (optimalChoice[i - 1][j / 10][k - cost[j]] == -40) {
					DP[i][j][k] = -40;
				} else {
					DP[i][j][k] = battleResult[i][j]
							+ optimalChoice[i - 1][j / 10][k - cost[j]];
				}

				for (int x = 0; x < pokemons / 10; x++) {
					if (x != j / 10) {
						optimalChoice[i][x][k] = max(optimalChoice[i][x][k],
								DP[i][j][k]);
					}
				}
			}
		}
	}

	return DP;
}

void createSubmission(int *submissionData, int enemies) {

	string filename = path + "/data/03_model_output/nn_model/Submission.csv";
	ifstream orig(path + "/data/01_raw/Submission.csv");
	ofstream output(filename);

	string line;
	getline(orig, line);
	output << line;

	for (int i = 0; i < enemies; i++) {
		getline(orig, line);
		line.pop_back();
		line.pop_back();
		output << (line + to_string(submissionData[i]) + "\n");
	}

	orig.close();
	output.close();

	cout << "Submission created in: " << filename << endl;
}

/* Backtracking the DP Table */
void backTrack(double ***DP, double **battleResults, int *cost, int enemies,
		int pokemons, int maxCost) {

	double totalHP = 0;

	int totalCost = 0;
	int maxIndex = 0;
	int currentCost = maxCost;

	int submissionData[enemies] = {0};

	for (int i = enemies - 1; i >= 0; i--) {

		int previousIndex = maxIndex;
		for (int j = 0; j < pokemons; j++) {
			if (DP[i][maxIndex][currentCost] <= DP[i][j][currentCost]) {
				maxIndex = j;
			}
		}

		if (previousIndex == maxIndex) {
			cerr << "Algorithm didn't work" << endl;
		}

		totalHP += battleResults[i][maxIndex] - 1;
		totalCost += cost[maxIndex];

		submissionData[i] = maxIndex;

		currentCost -= cost[maxIndex];
		cout << "Pokemon" << setfill(' ') << setw(5) << maxIndex
				<< " fights against Boss " << i << " and should have "
				<< setfill(' ') << setw(10) << battleResults[i][maxIndex] - 1
				<< " hp and cost " << cost[maxIndex] << endl;

	}

	cout << "Total HP Gained After fight is: " << totalHP << " at cost "
			<< totalCost << endl;
	cout << endl;

	createSubmission((int*) submissionData, enemies);
}

/* Reads Battle results from "files/battleResults.csv" */
double** getBattleResults(int enemyAmount, int pokemonAmount) {

	/* Allocate array for storing battleResults */
	double **battleResults = (double**) malloc(enemyAmount * sizeof(double*));
	for (int i = 0; i < enemyAmount; i++) {
		battleResults[i] = (double*) malloc(pokemonAmount * sizeof(double*));
	}

	string filename = path + "/data/03_model_output/nn_model/inference.csv";
	ifstream inputFile(filename);

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
	cout << "Read Battle Results from " << filename << endl;

	return battleResults;
}

/* Reads Battle results from "cost.csv" */
int* getCost(int pokemonAmount) {

	/* Allocate array for storing cost */
	int *cost = (int*) malloc(pokemonAmount * sizeof(int*));


	string filename = path + "/data/01_raw/cost.csv";
	ifstream inputFile(filename);
	cout << filename << endl;
	string line; //line i has i'th pokemon cost
	for (int i = 0; i < pokemonAmount; i++) {
		getline(inputFile, line);
		cost[i] = stoi(line);
	}

	inputFile.close();

	cout << "Read Cost from " << filename << endl;

	return cost;
}

int main(int argc, char **argv) {

	/* Manage all possible paths of input files */
	char temp[10000];
	path = getcwd(temp, sizeof(temp));
	path = path.substr(0, path.find("/team_creation"));

	/* Set values for reading files and anlyzing data*/
	int enemyAmount = 6;
	int pokemonAmount = 1440;
	int maxCost = 3500;

	/* Input */
	cout << "Obtaining Data " << endl;

	auto start = chrono::high_resolution_clock::now(); //time measurement

	int *cost = getCost(pokemonAmount);
	double **battleResults = getBattleResults(enemyAmount, pokemonAmount);

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