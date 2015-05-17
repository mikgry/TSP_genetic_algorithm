
#include "Menu.h"
#include "Clock.h"

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <math.h>

Menu::Menu() :
sa(tsp)
{
}


Menu::~Menu()
{
}

void Menu::start()
{
	tsp.readCitiesDataFromFileToCitiesMatrix("br17.atsp");

	Clock clock;

	//clock.startTimer();
	//sa.startAlgorithm(40, 40, 0.45);
	//clock.endTimer();
	//std::cout << std::endl << clock.ReturnTime() << std::endl << sa.getSolutionToString(sa.the_best_solution, sa.min_cost) << std::endl;
	int choice;
	while (true)
	{
		std::cout << "1. Wczytaj macierz miast z pliku." << std::endl;
		std::cout << "2. Wprowadz macierz miast z klawiatury." << std::endl;
		std::cout << "3. Wyswietl macierz miast." << std::endl;
		std::cout << "4. Uruchom algorytm." << std::endl;
		std::cout << "5. Uruchom algorytm w trybie rownoleglym na procesorze." << std::endl;
		std::cout << "6. Uruchom algorytm w trybie wykonywania pomiarów." << std::endl;
		std::cout << "7. Uruchom algorytm zrównoleglony w trybie wykonywania pomiarów." << std::endl;
		std::cout << "0. Zakoncz program.";
		std::cout << std::endl << "Wybierz opcje: ";

		std::cin >> choice;

		std::cout << std::endl << std::endl;

		switch (choice)
		{
			case 1:
				readCitiesMatrixFromFile();
				break;
			case 2:
				readCitiesMatrixFromKeyboard();
				break;
			case 3:
				printCitiesMatrix();
				break;
			case 4:
				startAlgorithm();
				break;
			case 5:
				startParallelAlgorithm();
				break;				
			case 6:
				startMeasurements();
				break;
			case 7:
				measureParallel();
				break;
				
			case 0:
				return;
		}
	}
}

void Menu::readCitiesMatrixFromFile()
{
	std::cout << "Podaj sciezke do pliku z macierza miast:\n";
	std::string path;
	std::cin >> path;

	if (tsp.readCitiesDataFromFileToCitiesMatrix(path))
	{
		std::cout << "Pomyslne wczytano macierz miast.\n";
	}
	else
	{
		std::cout << "Nie powiodlo sie wczytywanie macierzy miast z pliku.\n";
	}
}

void Menu::printCitiesMatrix()
{
	std::cout << std::endl << std::endl << tsp.getMatrixToString() << std::endl;
}

void Menu::readCitiesMatrixFromKeyboard()
{
	std::cout << "Podaj ilosc miast:\n";
	std::string cities_count;
	std::cin >> cities_count;
	int cost;
	int matrix_size = atoi(cities_count.c_str());
	tsp.setCitiesCount(matrix_size);

	for (int i = 0; i < matrix_size; i++)
	{
		for (int j = 0; j < matrix_size; j++)
		{
			if (i != j)
			{
				std::cout << "Podaj wartosc sciezki z miasta " << i << " do miasta " << j << "\t";
				std::cin >> cost;
				tsp.setEdgeCost(cost, i, j);
			}
				
		}
	}
}

void Menu::startAlgorithm()
{
	Clock clock; //tworzymy obiekt klasy zegar
	double time; //tworzymy zmienna do przetrzymywania zmierzonego czasu
	clock.startTimer(); //rozpoczynamy pomiar czasu
	sa.startAlgorithm(100, 0.4); //wywolujemy algorytm
	clock.endTimer(); //konczymy pomiar czasu
	time = clock.ReturnTime();
	std::cout << std::endl << sa.getSolutionToString(sa.the_best_solution, sa.min_cost) << std::endl << "Czas: " << time << std::endl;
}

void Menu::startParallelAlgorithm()
{
	Clock clock; //tworzymy obiekt klasy zegar
	double time; //tworzymy zmienna do przetrzymywania zmierzonego czasu
	clock.startTimer(); //rozpoczynamy pomiar czasu
	sa.startParallelAlgorithm(100, 0.4); //wywolujemy algorytm
	clock.endTimer(); //konczymy pomiar czasu
	time = clock.ReturnTime();
	std::cout << std::endl << sa.getSolutionToString(sa.the_best_solution, sa.min_cost) << std::endl << "Czas: " << time << std::endl;
}


void Menu::startAlgorithmStepByStep()
{
	//sa.startAlgorithmStepByStep(2, 3, 0.1);
	std::cout << std::endl << sa.getSolutionToString(sa.the_best_solution, sa.min_cost) << std::endl;
}

void Menu::startMeasurements()
{
	measureItr();
	//measureFinalTemp();
	//measureDelta();

}

void Menu::measureItr(){

	std::cout << "rozpoczynam pomiary";

	std::string matrix_file_name = "ftv44.atsp"; //ustalamy nazwe pliku wejsciowego

	tsp.readCitiesDataFromFileToCitiesMatrix(matrix_file_name); //wczytujemy instancje problemu

	Clock clock; //tworzymy obiekt klasy zegar
	double time; //tworzymy zmienna do przetrzymywania zmierzonego czasu

	std::string file_name = "E://wyniki_i_" + matrix_file_name + ".csv"; //ustalamy nazwe pliku wyjsciowego
	std::fstream file;

	file.open(file_name, std::ios::out);
	file << "solution" << " , " << "time" << std::endl; //nazywamy kolumny


	const int number_of_measurements = 100; //ustalamy ilosc eksperymentow


	for (int i = 0; i <= number_of_measurements; i++){

		clock.startTimer(); //rozpoczynamy pomiar czasu
		sa.startAlgorithm(100, 0.4); //wywolujemy algorytm
		clock.endTimer(); //konczymy pomiar czasu
		time = clock.ReturnTime();

		file << sa.min_cost << " , " << time << std::endl; //zapisujemy wyniki do pliku
	}

	file.close();
}


void Menu::measureParallel(){

	std::cout << "rozpoczynam pomiary";

	std::string matrix_file_name = "br17.atsp"; //ustalamy nazwe pliku wejsciowego

	tsp.readCitiesDataFromFileToCitiesMatrix(matrix_file_name); //wczytujemy instancje problemu

	Clock clock; //tworzymy obiekt klasy zegar
	double time; //tworzymy zmienna do przetrzymywania zmierzonego czasu

	std::string file_name = "E://wyniki_i_" + matrix_file_name + ".cpu.csv"; //ustalamy nazwe pliku wyjsciowego
	std::fstream file;

	file.open(file_name, std::ios::out);
	file << "solution" << " , " << "time" << std::endl; //nazywamy kolumny


	const int number_of_measurements = 100; //ustalamy ilosc eksperymentow


	for (int i = 0; i <= number_of_measurements; i++){

		clock.startTimer(); //rozpoczynamy pomiar czasu
		sa.startParallelAlgorithm(100, 0.4); //wywolujemy algorytm
		clock.endTimer(); //konczymy pomiar czasu
		time = clock.ReturnTime();

		file << sa.min_cost << " , " << time << std::endl; //zapisujemy wyniki do pliku
	}

	file.close();

}