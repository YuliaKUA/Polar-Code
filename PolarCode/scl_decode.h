#pragma once
#include "polar_code.h"

// ��������� ������� ���������������� ������(SC), ��������� ��������
// � ���������, �� ��������� LLR ��� ������� ��������������� ������ � ���������� ��� � �����
class SCLdecode {
public:
	SCLdecode();
	SCLdecode(PolarCode& code);
	~SCLdecode();

	std::vector<int> decode(std::vector<int>& message);

	double get_error(std::vector<int>& message, std::vector<int>& decode);

private:
	PolarCode code_;

	int N_;                                         // ����� �������� �����

	std::vector<int> message_received_;             // �������������� ���������
	
	void print(double p);
	void print(std::vector<int> vec);

};