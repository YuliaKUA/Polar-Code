#pragma once
#include <vector>
#include <iostream>

void print(std::vector<int>& vec) {
	for (int i = 0; i < vec.size(); i++) {
		std::cout << " " << vec[i] << " ";
	}
	std::cout << std::endl;
 }

void print(std::vector<double>& vec) {
	for (int i = 0; i < vec.size(); i++) {
		std::cout << " " << vec[i] << " ";
	}
	std::cout << std::endl;
}

void printp(std::vector<int>& vec) {
	for (int i = 0; i < vec.size(); i++) {
		std::cout << " " << vec[i] << ", ";
	}
	std::cout << std::endl;
}

void printd(std::vector<int>& vec) {
	for (int i = 0; i < vec.size(); i++) {
		std::cout << " " << i << ":" << vec[i] << "  ";
	}
	std::cout << std::endl;
}