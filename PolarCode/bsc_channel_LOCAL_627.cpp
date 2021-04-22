#include "bsc_channel.h"

#include <iostream>
#include <random>


BscChannel::BscChannel(float BER)
{
	BER_ = BER;
	rand_ = 0;
}

BscChannel::BscChannel()
{
	BER_ = 0;
	rand_ = 0;
}

BscChannel::~BscChannel()
{
}

BscChannel& BscChannel::operator=(BscChannel& right)
{
	if (this == &right) {
		return *this;
	}

	BER_ = right.BER_;
	rand_ = right.rand_;
<<<<<<< HEAD

=======
>>>>>>> ffe8c53751d7bcfd5d25c7e3d299cf9a8b64d027
	return *this;

}

double BscChannel::get_ber()
{
	return BER_;
}

std::vector<int> BscChannel::transmit(std::vector<int>& message)
{
	for (int i = 0; i < message.size(); i++) {
		error_pattern_.push_back(0);
	}

	double rand = 0;
	
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0.0, 1.0);

	for (int i = 0; i < message.size(); i++) {
		rand = dist(mt);
		//std:: cout << " " << rand << " ";
		if (rand < BER_) {
			error_pattern_[i] = 1;
		}
		else {
			error_pattern_[i] = 0;
		}
	}

	for (int i = 0; i < message.size(); i++) {
		error_pattern_[i] = message[i] ^ error_pattern_[i];
	}

	return error_pattern_;
}
