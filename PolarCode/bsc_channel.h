#pragma once
#include <vector>

    //BscChannel — реализация канальных методов для случая двоичного симметричного канала (BSC)
class BscChannel {
public:
	BscChannel();
	BscChannel(float BER);

	~BscChannel();

	BscChannel& operator= (BscChannel& right);

	double get_ber();

	std::vector<int> transmit(std::vector<int>& message);

private:
	// Достоверность передачи данных характеризует вероятность получить искажение для передаваемого бита данных
	float BER_;                //интенсивностью битовых ошибок (Bit Error Rate, BER)

	//double erasure_prob_;      //двойное начальное значение вероятности стирания первого неполяризованного канала BEC()

	double rand_;
						       
	std::vector<int> error_pattern_;
};