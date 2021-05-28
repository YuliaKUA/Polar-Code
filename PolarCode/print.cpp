#include "print.h"
#include <iostream>

void print(long double p)
{
	std::cout << p << std::endl;
}

void print(double p)
{
	std::cout << p << std::endl;
}

void print(int p)
{
	std::cout << p << std::endl;
}

void print(std::vector<int> vec)
{
	for (int i = 0; i < vec.size(); i++) {
		std::cout << vec[i] << " ";
	}
	std::cout << std::endl;
}
