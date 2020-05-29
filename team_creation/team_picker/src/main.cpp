#include <iostream>
#include <chrono>

#include "algorithm.h"
#include "io_handler.h"


using namespace std;

int main(int argc, char **argv) {

	setPath(argc, argv);

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
	int chosenPokemons[enemyAmount] = { 0 };
	backTrack(DP, battleResults, cost, enemyAmount, pokemonAmount, maxCost, chosenPokemons);

	finish = chrono::high_resolution_clock::now(); //time measurement
	elapsed = finish - start; //time measurement

	cout << "Time needed for analyzing data: " << elapsed.count()
			<< " seconds\n";

	createSubmission(chosenPokemons, enemyAmount);
	return 0;
}
