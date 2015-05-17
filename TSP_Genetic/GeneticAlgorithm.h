#pragma once
#include "TravelingSalesmanProblem.h"
#include <string>
#include <vector>
#include <list>
#include <mutex>

class GeneticAlgorithm
{
	void addCorrectCityAt(std::pair<std::vector<int>, int> &child, std::pair<std::vector<int>, int> const &parent, int position, int first, int second);
	int countCost(std::vector<int> path);
	void mutate(std::pair<std::vector<int>, int>& speciman, float probability);
	int rouletteWheelSelection();
	bool solutionIsUnique(std::pair<std::vector<int>, int> new_solution, std::vector<std::pair<std::vector<int>, int> > population);
	void swapTwoRandomCities(std::vector<int>& path);

	std::vector<std::pair<std::vector<int>, int> > population;
	TravelingSalesmanProblem& tsp;
	std::mutex population_mutex;
	
public:
	GeneticAlgorithm(TravelingSalesmanProblem& tsp);
	~GeneticAlgorithm();

	void crossoverPMX(int population_size, float mutation_probability);

	std::string getSolutionToString(std::vector<int> path, int cost);
	
	std::vector <int> generateRandomSolution();
	void startAlgorithm(int generation_count, float mutation_probability);
	void startParallelAlgorithm(int generation_count, float mutation_probability);

	void startAlgorithmStepByStep(int generation_count, float mutation_probability);
	
	std::vector< int > the_best_solution;
	int min_cost;
};