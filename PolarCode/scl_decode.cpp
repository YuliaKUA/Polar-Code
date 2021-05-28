#include "scl_decode.h"

SCLdecode::SCLdecode()
{
}

SCLdecode::SCLdecode(PolarCode& code)
{
	code_ = code;
	N_ = code.get_N();
}

SCLdecode::~SCLdecode()
{
}

std::vector<int> SCLdecode::decode(std::vector<int>& message)
{
	return std::vector<int>();
}

double SCLdecode::get_error(std::vector<int>& message, std::vector<int>& decode)
{
	double sum = 0;
	for (int i = 0; i < message.size(); i++) {
		if (message[i] != decode[i]) {
			sum++;
		}
	}

	return sum / N_;
}

void SCLdecode::print(double p)
{
	std::cout << p << std::endl;
}

void SCLdecode::print(std::vector<int> vec)
{
	for (int i = 0; i < vec.size(); i++) {
		std::cout << vec[i] << " ";
	}
	std::cout << std::endl;
}
