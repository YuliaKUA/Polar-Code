#pragma once
#include "polar_code.h"

// Реализует декодер последовательной отмены(SC), описанный Ариканом
// В частности, он вычисляет LLR для каждого поляризованного канала и сравнивает его с нулем
class SCLdecode {
public:
	SCLdecode();
	SCLdecode(PolarCode& code);
	~SCLdecode();

	std::vector<int> decode(std::vector<int>& message);

	double get_error(std::vector<int>& message, std::vector<int>& decode);

private:
	PolarCode code_;

	int N_;                                         // длина кодового слова

	std::vector<int> message_received_;             // декодированное сообшение
	
	void print(double p);
	void print(std::vector<int> vec);

};