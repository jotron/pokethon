#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;

string path = "";
string modelname = "final_model";

void createSubmission(int *submissionData, int enemies) {

	string filename = path + path + "/data/03_model_output/" + modelname + "/Submission.csv";
	ifstream orig(path + "/team_creation/input_data/Submission.csv");
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

double** getBattleResults(int enemyAmount, int pokemonAmount) {

	/* Allocate array for storing battleResults */
	double **battleResults = (double**) malloc(enemyAmount * sizeof(double*));
	for (int i = 0; i < enemyAmount; i++) {
		battleResults[i] = (double*) malloc(pokemonAmount * sizeof(double*));
	}

	string filename = path + "/data/03_model_output/" + modelname + "/inference.csv";
	ifstream inputFile(filename);

	if (!inputFile.is_open()) {
		cerr << "Model does not exist: " << modelname << endl;
		exit(-1);
	}
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

	string filename = path + "/team_creation/input_data/cost.csv";
	ifstream inputFile(filename);

	string line; //line i has i'th pokemon cost
	for (int i = 0; i < pokemonAmount; i++) {
		getline(inputFile, line);
		cost[i] = stoi(line);
	}

	inputFile.close();

	cout << "Read Cost from " << filename << endl;

	return cost;
}

void setPath(int argc, char **argv) {
	/* Manage all possible paths of input files */
	char temp[10000];
	path = getcwd(temp, sizeof(temp));
	path = path.substr(0, path.find("/team_creation"));

	if (argc > 1) {
		modelname = argv[1];
	}
}
