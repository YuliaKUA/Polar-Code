#include "awgn_bpsk_channel.h"
#include <iostream>
#include <math.h> 
#include <random>


AwgnBpskChannel::AwgnBpskChannel()
{
}

/* @brief Конструктор класса AWGN
	* @param SINR - конструкция SNR в децибелах
	* @param n - целочисленный показатель, который определяет длину кодового слова (N = 2^n)
	* @param K - число информационных битов */
AwgnBpskChannel::AwgnBpskChannel(const double &SINR, const int& n, const int& K)
{
	SINR_ = pow(10, SINR/10);    //SNR = 10*log10(A) дБ

	SINR_db_ = SINR;

	BER_ = (1 - erf(sqrt(SINR_))) / 2;         //см пределы интегрирования для erf и erfc(дополнительная функция ошибок)

	one_LLR_ = log(BER_ / (1 - BER_));
	zero_LLR_ = log((1 - BER_) / BER_);

	get_normalised_SINR(n, K);
}

AwgnBpskChannel::~AwgnBpskChannel()
{
}

AwgnBpskChannel& AwgnBpskChannel::operator=(AwgnBpskChannel& right)
{
	if (this == &right) {
		return *this;
	}

	n_ = right.n_;
	K_ = right.K_;

	BER_ = right.BER_;
	SINR_ = right.SINR_;
	SINR_db_ = right.SINR_db_; 
	norm_sinr_ = right.norm_sinr_;

	one_LLR_ = right.one_LLR_;
	zero_LLR_ = right.zero_LLR_;

	transmit_ = right.transmit_;
	likelihoods_ = right.likelihoods_;

	return *this;
	
}

/* @brief Возвращает значение BER
	* @return - BER */
double AwgnBpskChannel::get_ber()
{
	return BER_;
}

/* @brief Модуляция BPSK для битового поля - "0" соответствует +1, а "1" -1
	* @param message - закодированное сообщение
	* @return - cмодулированный сигнал с информацией из исходного сообщения */
std::vector<int> AwgnBpskChannel::modulate(std::vector<int>& message)
{
	//для sc_decode class
	for (int i = 0; i < message.size(); i++) {
		if (message[i] == 0) {
			message[i] = 1;
		}
		else {
			message[i] = -1;
		}
	}

	//для scd
	//"1" соответствует + sqrt(norm_sinr), а "0" - -sqrt(norm_sinr_)
	/*for (int i = 0; i < message.size(); i++) {
		if (message[i] == 0) {
			message[i] += sqrt(norm_sinr_);
		}
		else {
			message[i] -= sqrt(norm_sinr_);
		}
	}*/

	return message;
}


/* @brief Модуляция передачи сообщения по AWGN каналу (генерирует белый гауссовский шум)
	* @param message - закодированное смодулированное сообщение
	* @return - modulate_message + noise */
std::vector<double> AwgnBpskChannel::transmit(std::vector<int>& message)
{
	
	double noise_std = 1 / sqrt(2 * SINR_);
	
	//Данная функция возвращает случайные числа с плавающей точкой, которые выбраны из одномерного нормального (Гаусовского) распределения 
	//со средним значением равным 0 и дисперсией равной 2
	//2 параметр sqrt(sigma^2)
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0.0, 2.0);

	for (int i = 0; i < message.size(); i++) {
		transmit_.push_back((distribution(generator) * noise_std) + message[i]);
	}
	
	// для scd декодера
	//Находим логарифмическое отношение правдоподобия (LLR) для модулированного сообщения
	get_likelihoods(transmit_.size());

	return transmit_;
}

//не демодулировать
/* @brief Демодулируем наш сигнал с шумом
	* @param message - сигнал содержащий сообщение с шумом
	* @return - принятое сообщение */
std::vector<int> AwgnBpskChannel::demodulate(std::vector<double>& message)
{
	std::vector<int> result;
	for (int i = 0; i < message.size(); i++) {
		if (message[i] <= 0) {
			result.push_back(1);
		}
		else {
			result.push_back(0);
		}
	}

	return result;
}

/* @brief Возвращает вектор вычисленных LLR
	* @return - likelihoods vector */
std::vector<double> AwgnBpskChannel::get_likelihoods()
{
	return likelihoods_;
}

/* @brief Возвращает значения LLR для тривиальный случая одного поляризованного канала. Биту "0" соответствует "zero_LLR", а "1" - "one_LLR"
	*@param symb - принятый сигнал из гауссовского распределенного канала
	* @return - LLR */
long double AwgnBpskChannel::get_llr(int & symb)
{
	return (symb == 1 ? one_LLR_ : zero_LLR_);
}

//////////////////////////////////////////////////////////////////////////////
//Для scd декодера
/* @brief Инициализирует вектор likelihoods логарифмическими отношениями правдоподобия принятых сигналов с помощью функции LLR
	* @param len -  длина модулированного сообщения*/
void AwgnBpskChannel::get_likelihoods(int len)
{
	for (int i = 0; i < len; i++) {
		likelihoods_.push_back(LLR(transmit_[i]));
	}
}

/* @brief Вычисляет LLR логарифмическое отношение правдоподобия принятого сигнала
	* @param y - принятый сигнал из гауссовского распределенного канала
	* @return - LLR */
double AwgnBpskChannel::LLR(double y)           //LLR = Pr(y = 0) / Pr(y = 1)
{
	return -2 * y * sqrt(norm_sinr_) / 1;
}

/* @brief Нормализует E_b/N_o так, чтобы биты сообщения имели одинаковую мощность для любой скорости кода
	* @param n -  целочисленный показатель, который определяет длину кодового слова (N = 2^n) 
	* @param K -  число информационных битов*/
void AwgnBpskChannel::get_normalised_SINR(const int& n, const int& K)
{
	norm_sinr_ = SINR_ * K / pow(2, n);     // нормализованная мощность сигнала сообщения R = K / N
}

////////////////////////////////////////////////////////////////////////////////

//Функции вывода
void AwgnBpskChannel::print(std::vector<double>& vec)
{
	std::cout << std::endl;
	for (int i = 0; i < vec.size(); i++) {
		std::cout << vec[i] << ' ';
	}
	std::cout << std::endl;
}

void AwgnBpskChannel::print(double x)
{
	std::cout << std::endl << x << std::endl;
}
