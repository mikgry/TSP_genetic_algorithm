

#include "GeneticAlgorithm.h"
#include <algorithm>
#include <iostream>
#include <ctime>
#include <limits>
#include <thread>
#include <random>



//zwraca true jeœli first jest korzystniejszym rozwiazaniem niz second
struct PathComparator {
	bool operator() (std::pair<std::vector<int>, int> first, std::pair<std::vector<int>, int> second)
	{
		return (first.second < second.second);
	}
};

GeneticAlgorithm::GeneticAlgorithm(TravelingSalesmanProblem& t) :
tsp(t)
{
}


GeneticAlgorithm::~GeneticAlgorithm()
{
}

//przeprowadzenie ca³ego algorytmu
void GeneticAlgorithm::startAlgorithm(int generation_count, float mutation_probability)
{
	int population_size = tsp.getCitiesCount()*2;

	std::pair<std::vector<int>, int> first_solution, new_solution;
	population.clear();

	//pierwsze rozwi¹zanie wyznaczane jest losowo
	first_solution.first = generateRandomSolution();
	first_solution.second = countCost(first_solution.first);
	population.push_back(first_solution);

	//na pocz¹tku do populacji trafiaj¹ rozwi¹zania z s¹siedztwa typu swap pierwszego losowego rozwi¹zania 
	for (int i = 1; i < population_size; i++)
	{
		new_solution.first = generateRandomSolution();
		new_solution.second = countCost(new_solution.first);

		population.push_back(new_solution);
		
	}
	PathComparator comp;
	//rozwi¹zania s¹ sortowane od najlepszego do najgorszego
	std::sort(population.begin(), population.end(), comp);

	std::vector<std::pair<std::vector<int>, int> > next_generation;
	//ewolucja populacji przez wyznaczon¹ iloœæ pokoleñ
	for (int i = 0; i < generation_count; i++)
	{
		//kolejne pokolenie wyznaczone przez krzy¿owanie typu PMX
		crossoverPMX(population_size, mutation_probability, next_generation);

		//population = next_generation;
		//do populacji trafiaj¹ najlepsze rozwi¹zania z starej populacji i nowego pokolenia
		population.insert(population.end(), next_generation.begin(), next_generation.end());
		std::sort(population.begin(), population.end(), comp);
		population.resize(population_size);
	}

	the_best_solution = population[0].first;
	min_cost = population[0].second;
}

//wyznaczanie nastepnego pokolenia
void GeneticAlgorithm::crossoverPMX(int population_size, float mutation_probability, std::vector<std::pair<std::vector<int>, int> >& next_generation)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(0, population_size-1);

	next_generation.clear();
	std::pair<std::vector<int>, int> first_parent, second_parent, first_child, second_child;
	int first_cross_point, second_cross_point;
	//nowe pokolenie jest niemniejsze ni¿ populacja
	while (next_generation.size() < population_size)
	{
		//wybranie pary rozwi¹zañ metod¹ ruletki do krzy¿owania
		first_parent = population[dist(mt) % population.size()];
		second_parent = population[dist(mt) % population.size()];

		//wszystkie pola w œcie¿kach potomstwa wype³niam -1, wiem, ¿e miasto nie mo¿e mieæ takiego numeru
		first_child.first.resize(tsp.getCitiesCount(), -1);
		second_child.first.resize(tsp.getCitiesCount(), -1);

		//losowo s¹ wybierane punkty krzy¿owania
		first_cross_point = dist(mt) % tsp.getCitiesCount();
		// 		do
		// 		{
		second_cross_point = dist(mt) % tsp.getCitiesCount();
		// 		} while (first_cross_point == second_cross_point);


		//first_cross_point musi byæ bli¿ej pocz¹tku vectora
		if (first_cross_point > second_cross_point)
		{
			int buffor = first_cross_point;
			first_cross_point = second_cross_point;
			second_cross_point = buffor;
		}

		//do potomków trafiaj¹ na krzy¿ obszary krzy¿owania
		for (int j = first_cross_point; j <= second_cross_point; j++)
		{
			first_child.first[j] = second_parent.first[j];
			second_child.first[j] = first_parent.first[j];
		}

		//uzupe³nienie reszty œcie¿ki
		for (int j = 0; j < tsp.getCitiesCount(); j++)
		{
			//pomijam obszar krzy¿owania
			if (j == first_cross_point)
			{
				j = second_cross_point;
				continue;
			}

			//dodanie odpowiedniego miasta na j-tej pozycji
			addCorrectCityAt(first_child, first_parent, j, first_cross_point, second_cross_point);


			addCorrectCityAt(second_child, second_parent, j, first_cross_point, second_cross_point);

		}

		//mutacje potomków zachodz¹ z okreœlonym prawdopodobieñstwem
		mutate(first_child, mutation_probability);
		mutate(second_child, mutation_probability);

		//do nastêpnego pokolenia trafiaj¹ tylko rozwi¹zania nie znajduj¹ce siê w tym pokoleniu i w populacji rodzicielskiej
		first_child.second = countCost(first_child.first);

		second_child.second = countCost(second_child.first);
		//if (solutionIsUnique(first_child, next_generation) && solutionIsUnique(first_child, population)) next_generation.push_back(first_child);
		//if (solutionIsUnique(second_child, next_generation) && solutionIsUnique(second_child, population)) next_generation.push_back(second_child);

		next_generation.push_back(first_child);
		next_generation.push_back(second_child);
		first_child.first.clear();
		second_child.first.clear();
	}
}

//przeprowadzenie ca³ego algorytmu
void GeneticAlgorithm::startParallelAlgorithm(int generation_count, float mutation_probability)
{
	int population_size = tsp.getCitiesCount()*2;
	int const threads_count = 4;

	std::pair<std::vector<int>, int> first_solution, new_solution;
	population.clear();

	//pierwsze rozwi¹zanie wyznaczane jest losowo
	first_solution.first = generateRandomSolution();
	first_solution.second = countCost(first_solution.first);
	population.push_back(first_solution);

	//na pocz¹tku do populacji trafiaj¹ rozwi¹zania z s¹siedztwa typu swap pierwszego losowego rozwi¹zania 
	for (int i = 1; i < population_size; i++)
	{
		new_solution.first = generateRandomSolution();
		new_solution.second = countCost(new_solution.first);
		//do populacji trafiaj¹ tylko unikalne rozwi¹zania

		population.push_back(new_solution);
	}
	PathComparator comp;
	//rozwi¹zania s¹ sortowane od najlepszego do najgorszego
	std::sort(population.begin(), population.end(), comp);

	std::vector<std::pair<std::vector<int>, int> > next_generation[threads_count];
	//ewolucja populacji przez wyznaczon¹ iloœæ pokoleñ
	for (int i = 0; i < generation_count; i++)
	{
		//kolejne pokolenie wyznaczone przez krzy¿owanie typu PMX

		std::thread threads[threads_count];

		for (int i = 0; i < threads_count; i++)
		{
			threads[i] = std::thread(&GeneticAlgorithm::crossoverPMX, this, population_size/4, mutation_probability, std::ref(next_generation[i]));
		}

		for (int i = 0; i < threads_count; i++)
		{
			threads[i].join();
		}

		//population = next_generation;
		//do populacji trafiaj¹ najlepsze rozwi¹zania z starej populacji i nowego pokolenia
		for (int i = 0; i < threads_count; i++)
			population.insert(population.end(), next_generation[i].begin(), next_generation[i].end());
		std::sort(population.begin(), population.end(), comp);
		population.resize(population_size);
	}

	the_best_solution = population[0].first;
	min_cost = population[0].second;
}

void GeneticAlgorithm::addCorrectCityAt(std::pair<std::vector<int>, int> &child, std::pair<std::vector<int>, int> const &parent, int position, int first, int second)
{
	//wskaŸnik przesuniêcia w strefie krzy¿owania
	int shift = 0;
	//szerokoœæ przedzia³u krzy¿owania
	int interval = second - first + 1;

	//sprawdzenie, czy elemnt, który aktualnie ma byæ dodany, nie znajduje siê ju¿ w rozwi¹zaniu(by³ w obszarze krzy¿owania)
	int found_index = std::find(child.first.begin(),
		child.first.end(),
		parent.first[position])
		- child.first.begin();
	//jeœli nie znaleziono elementu w œcie¿ce
	if (found_index == child.first.size())
	{
		child.first[position] = parent.first[position];
	}
	else
	{
		//wyznaczenie elementu, który nale¿y dopisaæ ze strefy krzy¿owania drugiego osobnika
		while (std::find(child.first.begin(), child.first.end(), parent.first[first + (found_index - first + shift) % interval])
			!= child.first.end())
		{
			shift++;
		}
		child.first[position] = parent.first[first + (found_index - first + shift) % interval];
	}
}

//mutacja rozwi¹zania
void GeneticAlgorithm::mutate(std::pair<std::vector<int>, int>& speciman, float probability)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0, 100);
	std::uniform_int_distribution<int> swap_dist(0, population.size() / 4);
	//jeœli zostanie wylosowane prawdopodobieñstwo mniejsze od podanego, nastêpuje mutacja
	if (dist(mt)/100.0 / RAND_MAX < probability)
	{
		//wylosowana zostaje iloœæ ruchów swap w mutacji, maksymalnie wielkoœæ_populacji/4
		int swap_number = swap_dist(mt);
		for (int i = 0; i <= swap_number; i++)
		{
			swapTwoRandomCities(speciman.first);
		}
	}
}

//zwraca losow¹ permutacje miast
std::vector <int> GeneticAlgorithm::generateRandomSolution()
{
	std::vector <int> solution;
	for (int i = 0; i < tsp.getCitiesCount(); i++)
	{
		solution.push_back(i);
	}
	std::random_shuffle(solution.begin(), solution.end());
	return solution;
}

//liczy koszt przejœcia podanej œcie¿ki
int GeneticAlgorithm::countCost(std::vector<int> path)
{
	int cost = 0;
	for (std::size_t i = 0; i < path.size() - 1; i++)
	{
		cost += tsp.getEdgeCost(path[i], path[i + 1]);
	}
	cost += tsp.getEdgeCost(path[path.size() - 1], path[0]);

	return cost;
}

//zamienia miejscami dwa losowe miasta
void GeneticAlgorithm::swapTwoRandomCities(std::vector<int>& path)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(0, path.size()-1);

	int firstPosition = dist(mt);

	int secondPosition;
	do {
		secondPosition = dist(mt);
	} while (firstPosition == secondPosition);

	int buffor = path.at(firstPosition);
	path.at(firstPosition) = path.at(secondPosition);
	path.at(secondPosition) = buffor;
}

//zwraca rozwi¹zanie w formie do wyœwietlenia
std::string GeneticAlgorithm::getSolutionToString(std::vector<int> path, int cost)
{
	std::string str = "";
	str.append("Koszt przejscia sciezki:\n");
	str.append(std::to_string(cost));
	str.append("\nSciezka:\n");
	for (std::size_t i = 0; i < path.size(); i++)
	{
		str.append(std::to_string(path[i]));
		str.append(" ");
	}

	return str;
}