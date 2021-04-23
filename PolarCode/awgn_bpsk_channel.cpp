#include "awgn_bpsk_channel.h"
#include <iostream>
#include <math.h> 
#include <random>


AwgnBpskChannel::AwgnBpskChannel()
{
}

AwgnBpskChannel::AwgnBpskChannel(const double &SINR, const int& n, const int& K)
{
	SINR_ = pow(10, SINR/10);    //SINR = 10*log10(A) ไม

	SINR_db_ = SINR;

	BER_ = (1 - erf(sqrt(SINR_))) / 2;

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

	BER_ = right.BER_;
	SINR_ = right.SINR_;
	SINR_db_ = right.SINR_db_;

	one_LLR_ = right.one_LLR_;
	zero_LLR_ = right.zero_LLR_;

	return *this;
	
}

double AwgnBpskChannel::get_ber()
{
	return BER_;
}

std::vector<int> AwgnBpskChannel::modulate(std::vector<int>& message)
{
	//ไ๋ÿ scd class
	for (int i = 0; i < message.size(); i++) {
		if (message[i] == 0) {
			message[i] = 1;
		}
		else {
			message[i] = -1;
		}
	}

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

std::vector<double> AwgnBpskChannel::transmit(std::vector<int>& message, int& n, int& K)
{
	
	double noise_std = 1 / sqrt(2 * SINR_);
	
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0.0, 3.0);

	for (int i = 0; i < message.size(); i++) {
		transmit_.push_back(distribution(generator) * noise_std);
	}
	
	//get_likelihoods(transmit_.size());
	//print(likelihoods_);

	for (int i = 0; i < message.size(); i++) {
		transmit_[i] += message[i];
	}

	get_likelihoods(transmit_.size());
	//print(transmit_);

	return transmit_;
}

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

std::vector<double> AwgnBpskChannel::get_likelihoods()
{
	return likelihoods_;
}

long double AwgnBpskChannel::get_llr(int & symb)
{
	//print(one_LLR_);
	return (symb == 1 ? one_LLR_ : zero_LLR_);
}

double AwgnBpskChannel::LLR(double y)           //LLR = Pr(y = 0) / Pr(y = 1)
{
	return -2 * y * sqrt(norm_sinr_) / 1;
}

void AwgnBpskChannel::get_likelihoods(int len)
{
	for (int i = 0; i < len; i++) {
		likelihoods_.push_back(LLR(transmit_[i]));
	}
}

void AwgnBpskChannel::get_normalised_SINR(const int& n, const int& K)
{
	norm_sinr_ = SINR_ * K / pow(2, n);
	//print(norm_sinr_);
}

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
