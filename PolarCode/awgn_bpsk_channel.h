#pragma once
#include <vector>

//Ётот класс имитирует канал AWGN, добавл€€ гауссовский шум
//+ использует ћодул€цию BPSK дл€ битового пол€
class AwgnBpskChannel {
public:
	AwgnBpskChannel();
	AwgnBpskChannel(const double& SINR, const int& n, const int& K);

	~AwgnBpskChannel();

	AwgnBpskChannel& operator= (AwgnBpskChannel& right);

	double get_ber();

	std::vector<int> modulate(std::vector<int>& message);
	std::vector<double> transmit(std::vector<int>& message);
	std::vector<int> demodulate(std::vector<double>& message);

	std::vector<double> get_likelihoods();

	long double get_llr(int &symb);

private:
	// «атраты энергии на бит(отношение сигнал/шум SNR?)
	double SINR_;                       // обратное значение
	double SINR_db_;                    // при больших отношени€х сигнал/шум пропускна€ способность пропорциональна логарифму  10*log10(A) дЅ  

	// „астота битовых ошибок
	double BER_;                        // в случае модул€ции QPSK и канала AWGN BER как функци€ Eb/N0 задаетс€ 1/2 * erfc(sqrt(Eb / No))


	long double zero_LLR_;              // значение LLR дл€ тривиального случа€ одного пол€ризованного канала (входной символ 0)
	long double one_LLR_;               // значение LLR дл€ тривиального случа€ одного пол€ризованного канала (входной символ 1)

	double norm_sinr_;                  // дл€ SCD алгоритма, нормализованный sinr(дл€ одинаковой мощности)

	int n_;                             // целочисленный показатель, который определ€ет длину кодового слова (N = 2^n)
	int K_;                             // число информационных битов

	std::vector<double> transmit_;      // сообщение после модулировани€ передачи его по каналу
	double LLR(double y);

	std::vector<double> likelihoods_;   // дл€ SCD алгоритма, вектор вычисленных LLR
	void get_likelihoods(int len);

	void get_normalised_SINR(const int& n, const int& K);

	void print(std::vector<double>& vec);
	void print(double x);

};