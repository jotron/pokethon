#ifndef ALGORITHM_CPP_
#define ALGORITHM_CPP_

double*** getDPTable(double **battleResult, int *cost, int enemies,
		int pokemons, int maxCost);

void backTrack(double ***DP, double **battleResults, int *cost, int enemies,
		int pokemons, int maxCost);

#endif /* ALGORITHM_CPP_ */
