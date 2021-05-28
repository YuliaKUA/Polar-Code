#include "sc_decode.h"
#include "print.h"
#include <math.h>

SCdecode::SCdecode()
{
}

/* @brief ����������� ������ SCdecode
	* @param code - �������� ��� */
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

/* @brief ������� �������� ���������������� ������(SC), ���������� ��������
	* @param message - �������������� ��������������� ���������
	* @return - �������������� ���������*/
std::vector<int> SCdecode::decode(std::vector<double>& message)
{
	std::vector<int> temp_u_est;
	double llr = 0;

	//��� ������� ���� ���������� �� ������ ��������� LLR ���������������� ��������������� ������
	for (int i = 0; i < info_bit_position_.size(); i++) {
		temp_u_est.clear();

		for (int j = 0; j < info_bit_position_[i]; j++) {
			temp_u_est.push_back(u_est_[j]);
		}

		llr = recursive_llr(info_bit_position_[i], N_, message, temp_u_est);
		llrs_.push_back(llr);
		
		u_est_[info_bit_position_[i]] = (llr > 0 ? 0 : 1);
	}

	//print(u_est_);
	//������������� �������� ���������
	for (int i = 0; i < info_bit_position_.size(); i++) {
		message_received_.push_back(u_est_[info_bit_position_[i]]);
	}

	return message_received_;
}

/* @brief ��������� ����������� ������ ��� ���������� �������*/
double SCdecode::get_error(std::vector<int>& message, std::vector<int>& decode)
{
	double sum = 0;
	for (int i = 0; i < message.size(); i++) {
		if (message[i] != decode[i]) {
			sum++;
		}
	}

	return sum/N_;
}


/* @brief ���������� ��������� ��������������� ��������� ������������� (LLR) ��� i-�� ��������������� ������ �� N �� ������ ��������� ��������� y � ���������� ��������� �����
	* @param i - ������ ��������������� ������
	* @param N - ����� �������������� �������
	* @param y - ������ �����, ������������ ��� �������������
	* @param u_est - ������ ����� �������������� �����
	* @return - ������� �������� LLR ��� �������� ��������������� ������*/
long double SCdecode::recursive_llr(int i, int N, std::vector<double> y, std::vector<int> u_est)
{
	std::vector<int> temp_u_est;
	std::vector<double> temp_message;
	long double llr = 0, llr_1 = 0, llr_2 = 0;


	//����������� ������ ������ ��������������� ������(�� 1)
	if (i == 0 && N == 1) {
		llr = code_.channel_awgn_.get_llr(y[i]);
	}
	else {
		if (i % 2 == 0) {
			
			for (int j = 0; j < N / 2; j++) {
				temp_message.push_back(y[j]);
			}
			
			for (int j = 0; j < u_est.size(); j+=2) {
				temp_u_est.push_back(u_est[j] ^ u_est[j + 1]);
			}
			llr_1 = recursive_llr(i / 2, N / 2, temp_message, temp_u_est);
			

			temp_message.clear();
			temp_u_est.clear();
			
			for (int j = N / 2; j < y.size(); j++) {
				temp_message.push_back(y[j]);
			}
			for (int j = 1; j < u_est.size(); j += 2) {
				temp_u_est.push_back(u_est[j]);
			}
			
			llr_2 = recursive_llr(i / 2, N / 2, temp_message, temp_u_est);

			llr = llr_check_node_operation(llr_1, llr_2);
		}
		else {
			for (int j = 0; j < N / 2; j++) {
				temp_message.push_back(y[j]);
			}
			for (int j = 0; j < u_est.size() - 1; j += 2) {
				temp_u_est.push_back(u_est[j] ^ u_est[j + 1]);
			}
			llr_1 = recursive_llr((i - 1) / 2, N / 2, temp_message, temp_u_est);


			temp_message.clear();
			temp_u_est.clear();
	
			for (int j = N / 2; j < y.size(); j++) {
				temp_message.push_back(y[j]);
			}
			for (int j = 1; j < u_est.size(); j += 2) {
				temp_u_est.push_back(u_est[j]);
			}
			llr_2 = recursive_llr((i - 1) / 2, N / 2, temp_message, temp_u_est);


			llr = llr_2 + pow(-1, u_est[u_est.size() - 1]) * llr_1;
		}
	}

	//print(llr);
	return llr;
}

/* @brief ��������� LLR ��� ������ � tanh
	* @param llr_1 - ������� �������� ������� LLR
	* @param llr_2 - ������� �������� ������� LLR
	* @return - LLR
	*/
long double SCdecode::llr_check_node_operation(long double& llr_1, long double& llr_2)
{
	if (abs(llr_1) > CHECK_TANH && abs(llr_2) > CHECK_TANH) {
		//print(std::min(abs(llr_1), abs(llr_2)));
		if (llr_1 * llr_2 > 0) {
			return std::min(abs(llr_1), abs(llr_2));
		}
		else {
			return -1 * std::min(abs(llr_1), abs(llr_2));
		}
	}
	else {
		//print(2 * atan(tanh(llr_1 / 2) * tanh(llr_2 / 2)));
		return 2 * atanh(tanh(llr_1 / 2) * tanh(llr_2 / 2));
	}
}

