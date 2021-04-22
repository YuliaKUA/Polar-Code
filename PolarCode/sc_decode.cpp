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

		copy(message.end() - info_bit_position_[i], message.end(), back_inserter(temp_u_est));

		llr = slow_llr(info_bit_position_[i], N_, message, temp_u_est);
		u_est_[info_bit_position_[i]] = (llr > 0 ? 0 : 1);
	}

	return u_est_; // смодулировать выходное сообщение
}


long double SCdecode::slow_llr(int i, int N, std::vector<int> y, std::vector<int> u_est)
{
	std::vector<int> temp_u_est, temp_message;
	long double llr = 0, llr_1 = 0, llr_2 = 0;

	copy(y.end() - y.size() / 2, y.end(), back_inserter(temp_message));

	if (i == 0 && N == 1) {
		llr = code_.channel_awgn_.get_llr(message_[i]);
	}
	else {
		if (i % 2 == 0) {
			copy(y.end() - y.size() / 2, y.end(), back_inserter(temp_message));


			llr_1 = slow_llr(i / 2, N / 2, temp_message, u_est);

			temp_message.clear();
			copy(y.begin() + y.size() / 2, y.end(), back_inserter(temp_message));
			llr_2 = slow_llr(i / 2, N / 2, temp_message, u_est);


			llr = llr_check_node_operation(llr_1, llr_2);
		}
		else {
			copy(y.end() - y.size() / 2, y.end(), back_inserter(temp_message));
			llr_1 = slow_llr((i - 1) / 2, N / 2, temp_message, u_est);

			temp_message.clear();
			copy(y.begin() + y.size() / 2, y.end(), back_inserter(temp_message));
			llr_2 = slow_llr((i - 1) / 2, N / 2, temp_message, u_est);


			llr = llr_2 + pow(-1, u_est[u_est.size() - 1]) * llr_1;
		}
	}

	return llr;
}


long double SCdecode::llr_check_node_operation(long double& llr_1, long double& llr_2)
{
	if (abs(llr_1) > CHECK_NODE_TANH_THRES && abs(llr_2) > CHECK_NODE_TANH_THRES) {
		if (llr_1 * llr_2 > 0) {
			return std::min(abs(llr_1), abs(llr_2));
		}
		else {
			return -1 * std::min(abs(llr_1), abs(llr_2));
		}
	}
	else {
		return 2 * atanh(tanh(llr_1 / 2) * tanh(llr_2 / 2));
	}
}
