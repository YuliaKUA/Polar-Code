#pragma once
#include <vector>
#include <random>

//���� ����� ��������� ����� AWGN, �������� ����������� ���
//+ ���������� ��������� BPSK ��� �������� ����
class AwgnBpskChannel {
public:
	AwgnBpskChannel();
	AwgnBpskChannel(const double& SINR, const int& n, const int& K);

	~AwgnBpskChannel();

	AwgnBpskChannel& operator= (AwgnBpskChannel& right);

	double get_sinr();
	double get_Eb_No();

	std::vector<int> modulate(std::vector<int>& message);
	std::vector<double> transmit(std::vector<int>& message);
	std::vector<int> demodulate(std::vector<double>& message);

	std::vector<double> get_likelihoods();

	long double get_llr(double &symb);

private:
	// ������� ������� �� ���(��������� ������/��� SNR?)
	double SINR_;                       // �������� ��������
	double Es_No;                    // ��� ������� ���������� ������/��� ���������� ����������� ��������������� ���������  10*log10(A) ��  

	double R_;                          // �������� ������� ��������� R = K / N

	// ������� ������� ������
	long double BER_;                        // � ������ ��������� QPSK � ������ AWGN BER ��� ������� Eb/N0 �������� 1/2 * erfc(sqrt(Eb / No))


	long double zero_LLR_;              // �������� LLR ��� ������������ ������ ������ ��������������� ������ (������� ������ 0)
	long double one_LLR_;               // �������� LLR ��� ������������ ������ ������ ��������������� ������ (������� ������ 1)

	double norm_sinr_;                  // ��� SCD ���������, ��������������� sinr(��� ���������� ��������)

	int n_;                             // ������������� ����������, ������� ���������� ����� �������� ����� (N = 2^n)
	int K_;                             // ����� �������������� �����

	//��� ��������� ����
	double sigma_2_;                    // 2 �������� distribution 
	std::default_random_engine generator_;
	std::normal_distribution<double> distribution_;

	std::vector<double> transmit_;      // ��������� ����� ������������� �������� ��� �� ������
	double LLR(double y);

	std::vector<double> likelihoods_;   // ��� SCD ���������, ������ ����������� LLR
	void get_likelihoods(int len);

	void get_normalised_SINR(const int& n, const int& K);

};