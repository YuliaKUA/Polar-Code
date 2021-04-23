#include "sc_decode.h"
#include <math.h>

SCdecode::SCdecode()
{
}

SCdecode::SCdecode(PolarCode& code)
{
	code_ = code;

	info_bit_position_ = code.get_info_bit_position();
	N_ = code.get_N();

	for (int i = 0; i < code.get_N(); i++) {
		u_est_.push_back(0);
	}
}

SCdecode::~SCdecode()
{
}

std::vector<int> SCdecode::decode(std::vector<int>& message)
{
	std::vector<int> temp_u_est;

	message_ = message;
	double llr = 0;
	for (int i = 0; i < info_bit_position_.size(); i++) {
		temp_u_est.clear();

		for (int j = 0; j < info_bit_position_[i]; j++) {
			temp_u_est.push_back(u_est_[j]);
		}
		//print(temp_u_est);

		llr = slow_llr(info_bit_position_[i], N_, message, temp_u_est);
		llrs_.push_back(llr);
		u_est_[info_bit_position_[i]] = (llr > 0 ? 0 : 1);
	}

	for (int i = 0; i < info_bit_position_.size(); i++) {
		message_received_.push_back(u_est_[info_bit_position_[i]]);
	}

	return message_received_; // смодулировать выходное сообщение
}


long double SCdecode::slow_llr(int i, int N, std::vector<int> y, std::vector<int> u_est)
{
	//print(y);
	//std::cout << i << " " << N << std::endl;
	std::vector<int> temp_u_est, temp_message;
	long double llr = 0, llr_1 = 0, llr_2 = 0;

	//copy(y.end() - y.size() / 2, y.end(), back_inserter(temp_message));

	if (i == 0 && N == 1) {
		llr = code_.channel_awgn_.get_llr(y[i]);
		//std::cout << y[i] << " " << llr << std::endl;
	}
	else {
		if (i % 2 == 0) {
			//print(u_est);

			for (int j = 0; j < N / 2; j++) {
				temp_message.push_back(y[j]);
			}
			//copy(y.begin(), y.end() - (N / 2), back_inserter(temp_message));
			//print(temp_message);
			for (int j = 0; j < u_est.size(); j+=2) {
				temp_u_est.push_back(u_est[j] ^ u_est[j + 1]);
			}
			llr_1 = slow_llr(i / 2, N / 2, temp_message, temp_u_est);
			//print(temp_u_est);

			temp_message.clear();
			temp_u_est.clear();
			//copy(y.begin() + N / 2, y.end(), back_inserter(temp_message));
			for (int j = N / 2; j < y.size(); j++) {
				temp_message.push_back(y[j]);
			}
			for (int j = 1; j < u_est.size(); j += 2) {
				temp_u_est.push_back(u_est[j]);
			}
			
			llr_2 = slow_llr(i / 2, N / 2, temp_message, temp_u_est);

			//std::cout << llr_1 << " " << llr_2 << std::endl;
			llr = llr_check_node_operation(llr_1, llr_2);
		}
		else {
			//copy(y.begin(), y.end() - (N / 2), back_inserter(temp_message));
			for (int j = 0; j < N / 2; j++) {
				temp_message.push_back(y[j]);
			}
			for (int j = 0; j < u_est.size() - 1; j += 2) {
				temp_u_est.push_back(u_est[j] ^ u_est[j + 1]);
			}
			llr_1 = slow_llr((i - 1) / 2, N / 2, temp_message, temp_u_est);


			temp_message.clear();
			temp_u_est.clear();
			//copy(y.begin() + N / 2, y.end(), back_inserter(temp_message));
			for (int j = N / 2; j < y.size(); j++) {
				temp_message.push_back(y[j]);
			}
			for (int j = 1; j < u_est.size(); j += 2) {
				temp_u_est.push_back(u_est[j]);
			}
			llr_2 = slow_llr((i - 1) / 2, N / 2, temp_message, temp_u_est);


			llr = llr_2 + pow(-1, u_est[u_est.size() - 1]) * llr_1;
		}
	}

	//print(llr);
	return llr;
}


long double SCdecode::llr_check_node_operation(long double& llr_1, long double& llr_2)
{
	if (llr_1 * llr_2 > 0) {
		return std::min(abs(llr_1), abs(llr_2));
	}
	else {
		return -1 * std::min(abs(llr_1), abs(llr_2));
	}
}

void SCdecode::print(long double p)
{
	std::cout << p << std::endl;
}

void SCdecode::print(std::vector<int> vec)
{
	for (int i = 0; i < vec.size(); i++) {
		std::cout << vec[i] << " ";
	}
	std::cout << std::endl;
}
