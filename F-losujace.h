#pragma once
#include <random>

double rand1()//ta funkcja zwraca doubla wylosowanego w przedziale od 0.0 do 1.0
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	return dis(gen);
}

double rand2(double min, double max)//ta funkcja zwraca double wylosowanego w przedzialane zadanym w argumentach
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(min, max);
	return dis(gen);
}

int rand3(int min,int max)// ta funkcja zwraca inta wylosowanego w przedziale zadanym w argumentach z granicami w³¹cznie
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(min, max);
	return distrib(gen);
}