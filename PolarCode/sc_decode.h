#pragma once
#include "polar_code.h"

// –еализует декодер последовательной отмены(SC), описанный јриканом
// ¬ частности, он вычисл€ет LLR дл€ каждого пол€ризованного канала и сравнивает его с нулем
class SCdecode {
public:
	SCdecode();
	SCdecode(PolarCode& code);
	~SCdecode();

	std::vector<int> decode(std::vector<double>& message);

	double get_error(std::vector<int>& message, std::vector<int>& decode);

private:
	PolarCode code_;                                

	std::vector<int> u_est_;                        // целочисленный вектор ранее декодированных битов
	std::vector<int> info_bit_position_;            // вектор информационных битовых индексов в кодовом слове
	int N_;                                         // длина кодового слова

	std::vector<int> message_received_;             // декодированное сообшение
	//–екурсивные отношени€ алгоритмов в пол€рном кодировании требуют, чтобы мы работали в log-domain, чтобы избежать ошибок переполнени€ в их реализаци€х.
	std::vector<long double> llrs_;                 // вектор LLR

	long double recursive_llr(int i, int N, std::vector<double> y, std::vector<int> u_est);
	long double llr_check_node_operation(long double& llr_1, long double& llr_2);

	int CHECK_TANH = 40;
};