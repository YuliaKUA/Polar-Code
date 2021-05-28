#include "awgn_bpsk_channel.h"
#include "print.h"

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
AwgnBpskChannel::AwgnBpskChannel(const double &Eb_No, const int& n, const int& K)
{
	R_ = K / pow(2, n);
	//SINR_ = Eb_No * R_;

	SINR_ = pow(10, Eb_No/10);    //SNR = 10*log10(A) дБ

	Es_No = Eb_No * R_;
	
	//sigma
	//gen noize

	//BER_ = (1 - erf(Eb_No)) / 2;         //см пределы интегрирования для erf и erfc(дополнительная функция ошибок)

	//delete
	//one_LLR_ = log(BER_ / (1 - BER_));
	//zero_LLR_ = log((1 - BER_) / BER_);
	

	//get_normalised_SINR(n, K);

	//Данная функция возвращает случайные числа с плавающей точкой, которые выбраны из одномерного нормального (Гаусовского) распределения 
	//со средним значением равным 0
	//2 параметр sqrt(sigma^2)
	sigma_2_ = 0.5 * (1 / get_Eb_No()) * (1 / R_); //один раз определять
	std::normal_distribution<double> distribution(0.0, sqrt(sigma_2_));
	distribution_ = distribution;
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
	Es_No = right.Es_No; 
	norm_sinr_ = right.norm_sinr_;

	one_LLR_ = right.one_LLR_;
	zero_LLR_ = right.zero_LLR_;

	transmit_ = right.transmit_;
	likelihoods_ = right.likelihoods_;

	return *this;
	
}

/* @brief Возвращает значение SINR
	* @return - SINR */
double AwgnBpskChannel::get_sinr()
{
	return SINR_;
}

double AwgnBpskChannel::get_Eb_No()
{
	return Es_No / R_;
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

	return message;
}


/* @brief Модуляция передачи сообщения по AWGN каналу (генерирует белый гауссовский шум)
	* @param message - закодированное смодулированное сообщение
	* @return - modulate_message + noise */
std::vector<double> AwgnBpskChannel::transmit(std::vector<int>& message)
{	
	print(sqrt(sigma_2_));

	for (int i = 0; i < message.size(); i++) {
		transmit_.push_back((distribution_(generator_)) + message[i]);
	}

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
long double AwgnBpskChannel::get_llr(double & symb)
{
	return -2 * symb / sigma_2_;
	//return (symb == 1 ? one_LLR_ : zero_LLR_);
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

///////////////////////////////////////////////////////////////////////////////
