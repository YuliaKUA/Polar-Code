#pragma once
#include "polar_code.h"

// ��������� ������� ���������������� ������(SC), ��������� ��������
// � ���������, �� ��������� LLR ��� ������� ��������������� ������ � ���������� ��� � �����
class SCdecode {
public:
	SCdecode();
	SCdecode(PolarCode& code);
	~SCdecode();

	std::vector<int> decode(std::vector<double>& message);

	double get_error(std::vector<int>& message, std::vector<int>& decode);

private:
	PolarCode code_;                                

	std::vector<int> u_est_;                        // ������������� ������ ����� �������������� �����
	std::vector<int> info_bit_position_;            // ������ �������������� ������� �������� � ������� �����
	int N_;                                         // ����� �������� �����

	std::vector<int> message_received_;             // �������������� ���������
	//����������� ��������� ���������� � �������� ����������� �������, ����� �� �������� � log-domain, ����� �������� ������ ������������ � �� �����������.
	std::vector<long double> llrs_;                 // ������ LLR

	long double recursive_llr(int i, int N, std::vector<double> y, std::vector<int> u_est);
	long double llr_check_node_operation(long double& llr_1, long double& llr_2);

	int CHECK_TANH = 40;
};