#pragma once
#include <vector>

//���� ����� ��������� ����� AWGN, �������� ����������� ���
//+ ���������� ��������� BPSK ��� �������� ����
class AwgnBpskChannel {
public:
	AwgnBpskChannel();
	AwgnBpskChannel(const double& SINR, const int& n, const int& K);

	~AwgnBpskChannel();

	AwgnBpskChannel& operator= (AwgnBpskChannel& right);

	double get_ber();

	std::vector<int> modulate(std::vector<int>& message);
	std::vector<double> transmit(std::vector<int>& message);
	std::vector<int> demodulate(std::vector<double>& message);

	std::vector<double> get_likelihoods();

	long double get_llr(int &symb);

private:
	// ������� ������� �� ���(��������� ������/��� SNR?)
	double SINR_;                       // �������� ��������
	double SINR_db_;                    // ��� ������� ���������� ������/��� ���������� ����������� ��������������� ���������  10*log10(A) ��  

	// ������� ������� ������
	double BER_;                        // � ������ ��������� QPSK � ������ AWGN BER ��� ������� Eb/N0 �������� 1/2 * erfc(sqrt(Eb / No))


	long double zero_LLR_;              // �������� LLR ��� ������������ ������ ������ ��������������� ������ (������� ������ 0)
	long double one_LLR_;               // �������� LLR ��� ������������ ������ ������ ��������������� ������ (������� ������ 1)

	double norm_sinr_;                  // ��� SCD ���������, ��������������� sinr(��� ���������� ��������)

	int n_;                             // ������������� ����������, ������� ���������� ����� �������� ����� (N = 2^n)
	int K_;                             // ����� �������������� �����

	std::vector<double> transmit_;      // ��������� ����� ������������� �������� ��� �� ������
	double LLR(double y);

	std::vector<double> likelihoods_;   // ��� SCD ���������, ������ ����������� LLR
	void get_likelihoods(int len);

	void get_normalised_SINR(const int& n, const int& K);

	void print(std::vector<double>& vec);
	void print(double x);

};