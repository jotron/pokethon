#ifndef ALGORITHM_CPP_
#define ALGORITHM_CPP_

double*** getDPTable(double **battleResult, int *cost, int enemies,
		int pokemons, int maxCost);

int* backTrack(double ***DP, double **battleResults, int *cost, int enemies,
		int pokemons, int maxCost, int* submissionData);

#endif /* ALGORITHM_CPP_ */
