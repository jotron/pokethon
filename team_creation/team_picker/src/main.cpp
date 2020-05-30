#include <iostream>
#include <chrono>

#include "algorithm.h"
#include "io_handler.h"


using namespace std;

int main(int argc, char **argv) {

	/* Set path for file reading */
	setPath(argc, argv);

	/* Set values for reading files and analyzing data*/
	int champions = 6;
	int pokemons = 1440;
	int maxCost = 3500;

	/* Input */
	cout << "Obtaining Data " << endl;

	auto start = chrono::high_resolution_clock::now(); //time measurement

	int *cost = getCost(pokemons);
	double **battleResults = getBattleResults(champions, pokemons);

	auto finish = chrono::high_resolution_clock::now(); //time measurement
	chrono::duration<double> elapsed = finish - start; //time measurement

	cout << "Time needed for obtaining data: " << elapsed.count()
			<< " seconds\n" << endl;

	/* Analysis */
	cout << "Analyzing Data " << endl;

	start = chrono::high_resolution_clock::now(); //time measurement

	/* DP algorithm */
	double ***DP = getDPTable(battleResults, cost, champions, pokemons,
			maxCost);
	int *chosenPokemons = (int*) malloc(champions * sizeof(int));
	backTrack(DP, battleResults, cost, champions, pokemons, maxCost, chosenPokemons);

	finish = chrono::high_resolution_clock::now(); //time measurement
	elapsed = finish - start; //time measurement

	cout << "Time needed for analyzing data: " << elapsed.count()
			<< " seconds\n";

	createSubmission(chosenPokemons, champions);
	return 0;
}
