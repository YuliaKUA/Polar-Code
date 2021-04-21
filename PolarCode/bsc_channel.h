#pragma once
#include <vector>

    //BscChannel � ���������� ��������� ������� ��� ������ ��������� ������������� ������ (BSC)
class BscChannel {
public:
	BscChannel();
	BscChannel(float BER);

	~BscChannel();

	BscChannel& operator= (BscChannel& right);

	double get_ber();

	std::vector<int> transmit(std::vector<int>& message);

private:
	// ������������� �������� ������ ������������� ����������� �������� ��������� ��� ������������� ���� ������
	float BER_;                //�������������� ������� ������ (Bit Error Rate, BER)

	//double erasure_prob_;      //������� ��������� �������� ����������� �������� ������� ����������������� ������ BEC()

	double rand_;
						       
	std::vector<int> error_pattern_;
};