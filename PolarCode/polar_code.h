#pragma once
#include "bsc_channel.h"
#include "awgn_bpsk_channel.h"
#include <iostream>
#include <vector>



//реализуем концепцию полярного кода
class PolarCode {
private:
	int n_;                                        //целочисленный показатель, который определяет длину кодового слова (N = 2^n)
	int K_;                                        //число информационных битов
	int N_;                                        //длина кодового слова

	
	std::vector<int> info_bits_positions_;         //вектор информационных битовых индексов в кодовом слове
	std::vector<int> frozen_bits_positions_;       //вектор индексов замороженных битов (равных 0) в кодовом слове
	
	std::vector<double> bhatt_z_array_;
	std::vector<double> bhatt_z_array_non_rec_;
	std::vector<int> bhatt_z_array_index_;

	std::vector<int> u_message_;
	std::vector<int> x_message_;

	std::vector<std::vector<int>> B_;
	std::vector<std::vector<double>> L_;
	std::vector<double> likelihoods_;

	void initializing();

	void construct();                              //конструирует код, т. е. определяет, какие биты являются информационными, а какие-замороженными
	void dependent_bec_construction();
	double bhatt_z(int i, int N);
	double bhatt_z(int i);

	void init_bhat_non_rec();

	void extend_info_bits(std::vector<int>& info_bits);
	std::vector<int> polar_transform(std::vector<int> u_message);

	void bit_reversed();
	std::vector<int> bit_reversed_;

	void update_llrs(int &l);
	int active_llr_level(int& l);

	double curr_ber_;

	double upper_llr(double &l1, double& l2);
	double logdomain_sum(const double& x, const double& y);

	double lower_llr(double &l1, double &l2, int& b);

	void update_bits(int& l);
	int active_bit_level(int& i);

	void form_B();
	std::vector<int> form_B_;
	void form_message_received();
	std::vector<int> message_received_;

	void sort();
	void print(std::vector<double>& vec);
	void print(std::vector<int>& vec);
	void printf(std::vector<int>& vec);
	void print(std::vector<std::vector<double>>& vec);
	void print(double x);
	void print(int x);

public:
	PolarCode();
	PolarCode(int& n, int& K, BscChannel& channel);
	PolarCode(int& n, int& K, AwgnBpskChannel& channel);

	BscChannel channel_;
	AwgnBpskChannel channel_awgn_;
	
	~PolarCode();
	std::vector<int> encode(std::vector<int>& message);
	std::vector<int> decode(std::vector<int>& message);

	void receive_likelihoods(const std::vector<double>& likelihoods);
	int &get_N();
	std::vector<int>& get_info_bit_position();

	PolarCode& operator= (PolarCode& right);
};