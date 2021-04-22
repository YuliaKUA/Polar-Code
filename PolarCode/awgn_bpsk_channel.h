#pragma once
#include <vector>

class AwgnBpskChannel {
public:
	AwgnBpskChannel();
	AwgnBpskChannel(const double& SINR, const int& n, const int& K);

	~AwgnBpskChannel();

	AwgnBpskChannel& operator= (AwgnBpskChannel& right);

	double get_ber();

	std::vector<int> modulate(std::vector<int>& message);
	std::vector<double> transmit(std::vector<int>& message, int& n, int& K);
	std::vector<int> demodulate(std::vector<double>& message);

	std::vector<double> get_likelihoods();


private:
	double SINR_;
	double SINR_db_;
	double BER_;

	double norm_sinr_;

	int n_;
	int K_;

	std::vector<double> transmit_;
	double LLR(double y);

	std::vector<double> likelihoods_;
	void get_likelihoods(int len);

	void get_normalised_SINR(const int& n, const int& K);

	void print(std::vector<double>& vec);
	void print(double x);

};