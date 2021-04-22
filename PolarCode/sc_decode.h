#pragma once
#include "polar_code.h"

class SCdecode {
public:
	SCdecode();
	SCdecode(PolarCode& code);
	~SCdecode();

	std::vector<int> decode(std::vector<int>& message);

private:
	PolarCode code_;

	std::vector<int> u_est_;
	std::vector<int> info_bit_position_;
	int N_;

	std::vector<int> message_;

	long double slow_llr(int i, int N, std::vector<int> y, std::vector<int> u_est);
	long double llr_check_node_operation(long double& llr_1, long double& llr_2);

	int CHECK_NODE_TANH_THRES = 44;
};