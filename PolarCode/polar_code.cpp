#include "polar_code.h"
#include <algorithm>

PolarCode::PolarCode()
{
}

PolarCode::PolarCode(int& n, int& K, BscChannel& channel)
{
	n_ = n;
	K_ = K;
	N_ = pow(2, n);

	channel_ = channel;
	curr_ber_ = channel_.get_ber();

	info_bits_positions_.reserve(K_);
	frozen_bits_positions_.reserve(N_ - K_);

	bhatt_z_array_.reserve(N_);
	bhatt_z_array_index_.reserve(N_);

	u_message_.reserve(N_);
	x_message_.reserve(N_);

	bhatt_z_array_non_rec_.reserve(N_);
	init_bhat_non_rec();
	
	//определяем, какие биты являются информационными, а какие-замороженными
	construct();
	
}

PolarCode::PolarCode(int& n, int& K, AwgnBpskChannel& channel)
{
	n_ = n;
	K_ = K;
	N_ = pow(2, n);

	channel_awgn_ = channel;
	curr_ber_ = channel_awgn_.get_ber();

	info_bits_positions_.reserve(K_);
	frozen_bits_positions_.reserve(N_ - K_);

	bhatt_z_array_.reserve(N_);
	bhatt_z_array_index_.reserve(N_);

	u_message_.reserve(N_);
	x_message_.reserve(N_);

	bhatt_z_array_non_rec_.reserve(N_);
	init_bhat_non_rec();
	
	initializing();

	//определяем, какие биты являются информационными, а какие-замороженными
	construct();
	//printf(frozen_bits_positions_);
}

PolarCode::~PolarCode()
{
}

void PolarCode::initializing()
{
	std::vector<int> temp(n_+1, -1);
	std::vector<double> temp2(n_+1, -1);

	for (int i = 0; i < N_; i++) {
		B_.push_back(temp);
		L_.push_back(temp2);
	}
}

//Конструирует код, т. е. определяет, какие биты являются информационными, а какие-замороженными
void PolarCode::construct()
{
	dependent_bec_construction();
	//print(bhatt_z_array_index_);

	//возьмем первые K битов в качестве информационных
	for (int i = 0; i < K_; i++) {
		info_bits_positions_.push_back(bhatt_z_array_index_[i]);
	}
	std::sort(info_bits_positions_.begin(), info_bits_positions_.end());

	//Оставшиеся биты - замороженные
	for (int i = K_; i < N_; i++) {
		frozen_bits_positions_.push_back(bhatt_z_array_index_[i]);
	}
	std::sort(frozen_bits_positions_.begin(), frozen_bits_positions_.end());
	//print(frozen_bits_positions_);
}

//Реализует метод BEC полярного построения. 
//Инициализирует первый канал с вероятностью стирания равной BER вместо 0,5
void PolarCode::dependent_bec_construction()
{
	//вычисляем Z-параметр для каждого из N поляризованных каналов
	for (int i = 0; i < N_; i++) {
		bhatt_z_array_.push_back(bhatt_z(i, N_));
		bhatt_z_array_index_.push_back(i);
	}

	/*print(bhatt_z_array_);
	print(bhatt_z_array_non_rec_);*/
	//std::cout << "Is? " << (bhatt_z_array_ == bhatt_z_array_non_rec_) << std::endl;

	//сортируем все каналы в порядке возрастания их Z-параметра, так как чем меньше Z-параметр, 
	//тем более подходящим для передачи информации является этот канал
	//std::sort(bhatt_z_array_.begin(), bhatt_z_array_.end());
	sort();
}


//Bubble sort
void PolarCode::sort()
{
	double tmp = 0;
	int tmp2 = 0;
	for (int i = 0; i < N_; i++) {
		for (int j = (N_ - 1); j >= (i + 1); j--) {
			if (bhatt_z_array_[j] < bhatt_z_array_[j - 1]) {
				tmp = bhatt_z_array_[j];
				tmp2 = bhatt_z_array_index_[j];

				bhatt_z_array_[j] = bhatt_z_array_[j - 1];
				bhatt_z_array_index_[j] = bhatt_z_array_index_[j - 1];

				bhatt_z_array_[j - 1] = tmp;
				bhatt_z_array_index_[j - 1] = tmp2;
			}
		}
	}
}

//Рекурсивно вычисляется значение Z - параметра Бхаттачарьи для i - го из N поляризованных каналов.
//Расчет основан на соотношениях, полученных в оригинальной работе Арикана.
//исправить -> 1 раз считать (z)^2
double PolarCode::bhatt_z(int i, int N)
{

	if (i == 0 && N == 1) {
		//std::cout << "*" << std::endl;
		return curr_ber_;
	}
	else if (i % 2 == 0) {
		double temp = 2 * bhatt_z(i / 2, N / 2) - pow(bhatt_z(i / 2, N / 2), 2);
		//std::cout << i << " " << temp << std:: endl;
		return temp;
	}
	else {
		double temp2 = pow(bhatt_z((i - 1) / 2, N / 2), 2);
		//std:: cout << i << " " << temp2 << std::endl;
		return temp2;
	}
}

double PolarCode::bhatt_z(int i) {
	//std::cout << rec.size();
	return bhatt_z_array_non_rec_[i];
}

void PolarCode::init_bhat_non_rec()
{
	int N = 0;
	int i = 0;
	double k = 0;
	double k_pred = 0;
	std::vector<int> temp;
	int iter = 0;

	for (int j = 0; j < N_; j++) {
		i = j;
		N = log2(N_);
		temp.clear();

		temp.push_back(i);
		while (N > 0 || i != 0) {
			if (i % 2 == 0) {
				i /= 2;
			}
			else {
				i = (i - 1) / 2;
			}
			N--;
			temp.push_back(i);
		}

		
		for (int j = temp.size() - 1 ; j >= 0; j--) {
			if (j == temp.size() - 1) {
				k_pred = k;
				k = curr_ber_;
			}
			else if (temp[j] % 2 == 0) {
				k_pred = k;
				k = 2 * k_pred - pow(k_pred, 2);
			}
			else {
				k_pred = k;
				k = pow(k_pred, 2);
			}
		}

		bhatt_z_array_non_rec_.push_back(k);
	}

	//print(bhatt_z_array_non_rec_);
}

//Систематическое кодирование
//Кодирует K информационных битов в N битов сообщения кодового слова путем заполнения
//их с замороженными битами и с помощью полярного преобразования после этого
std::vector<int> PolarCode::encode(std::vector<int>& message)
{
	extend_info_bits(message);
	x_message_ = polar_transform(u_message_);

	return x_message_;
}


void PolarCode::extend_info_bits(std::vector<int>& info_bits)
{
	if (info_bits.size() != K_) {
		std::cout << "Не удалось закодировать сообщение. Некорректная длина кодируемого сообщения.";
		return;
	}

	for (int i = 0; i < N_; i++) {
		u_message_.push_back(0);
	}

	// установим значения информационных битов в u_message
	//(на позиции info_bits_positions_ устанавливаем значения info_bits)
	for (int i = 0; i < K_; i++) {
		u_message_[info_bits_positions_[i]] = info_bits[i];
	}

	//print(u_message_);
}

//Реализует полярное преобразование данного сообщения рекурсивным способом
std::vector<int> PolarCode::polar_transform(std::vector<int> u_message)
{
	std::vector<int> x_message, temp, u1u2, u2;

	if (u_message.size() == 1) {
		x_message = u_message;
	}
	else {		
		for (int i = 0; i < u_message.size(); i+=2) {
			u1u2.push_back(u_message[i] ^ u_message[i + 1]);
			u2.push_back(u_message[i + 1]);
		}

		x_message = polar_transform(u1u2);
		temp = polar_transform(u2);

		for (int i = 0; i < temp.size(); i++) {
			x_message.push_back(temp[i]);
		}
		//x_message.insert(x_message_.end(), temp.begin(), temp.end());
	}

	return x_message;
}


std::vector<int> PolarCode::decode(std::vector<int>& message)
{
	for (int i = 0; i < N_; i++) {
		L_[i][0] = likelihoods_[i];
	}
	

	bit_reversed();

	for (int l = 0; l < N_; l++) {
		update_llrs(bit_reversed_[l]);

		if (std::binary_search(frozen_bits_positions_.begin(), frozen_bits_positions_.end(), bit_reversed_[l])) {
			B_[bit_reversed_[l]][n_] = 0;
		}
		else {
			B_[bit_reversed_[l]][n_] = ((L_[bit_reversed_[l]][n_]) >= 0 ? 0 : 1);
		}
		
		update_bits(bit_reversed_[l]);
	}

	//print(info_bits_positions_);

	form_B();
	form_message_received();

	return message_received_;
}

void PolarCode::form_B()
{
	for (int i = 0; i < N_; i++) {
		form_B_.push_back(B_[i][n_]);
	}
}

void PolarCode::form_message_received()
{
	for (int i = 0; i < N_; i++) {
		if (!std::binary_search(frozen_bits_positions_.begin(), frozen_bits_positions_.end(), i)) {
			message_received_.push_back(form_B_[i]);
		}
	}
}

void PolarCode::receive_likelihoods(const std::vector<double>& likelihoods)
{
	likelihoods_ = likelihoods;
}

void PolarCode::bit_reversed()
{
	int result = 0;
	for (int i = 0; i < N_; i++) {
		result = 0;
		for (int j = 0; j < n_; j++) {
			if (i & (1 << j)) {
				result |= (1 << (n_ - 1 - j));
			}
		}
		bit_reversed_.push_back(result);
	}

	//print(bit_reversed_);
}

int PolarCode::active_llr_level(int& l)
{
	int mask = pow(2, (n_ - 1));
	int count = 1;
	for (int k = 0; k < n_; k++) {
		if ((mask & l) == 0) {
			count += 1;
			mask = mask >> 1;
		}
		else {
			break;
		}
	}
	return std::min(count, n_);
}

void PolarCode::update_llrs(int& l)
{
	int active_llr_level = this->active_llr_level(l);
	int block_size = 0, branch_size = 0, top_bit = 0;
	double top_llr = 0, btm_llr = 0;

	for (int s = n_- active_llr_level; s < n_; s++) {
		//print(active_llr_level);
		block_size = pow(2, (s + 1));
		branch_size = block_size / 2;

		for (int j = l; j < N_; j += block_size) {
			if (j % block_size < branch_size) {
				top_llr = L_[j][s];
				btm_llr = L_[j + branch_size][s];
				L_[j][s + 1] = upper_llr(top_llr, btm_llr);
			}
			else {
				btm_llr = L_[j][s];
				top_llr = L_[j - branch_size][s];
				top_bit = B_[j - branch_size][s + 1];
				L_[j][s + 1] = lower_llr(btm_llr, top_llr, top_bit);
			}
		}
	}
}

double PolarCode::upper_llr(double& l1, double& l2)
{
	return logdomain_sum(l1 + l2, 0) - logdomain_sum(l1, l2);
}

double PolarCode::logdomain_sum(const double& x, const double& y)
{
	if (x > y) {
		return x + log1p(exp(y - x));
	}
	else {
		return y + log1p(exp(x - y));
	}
}

double PolarCode::lower_llr(double& l1, double& l2, int& b)
{
	if (b == 0) {                                              
		return l1 + l2;                                         
	}
	else {
		return l1 - l2;
	}
}

void PolarCode::update_bits(int& l)
{
	if (l < N_ / 2) {
		return;
	}
	int active_bit_level = this->active_bit_level(l);
	int block_size = 0, branch_size = 0;
	for (int s = n_; s > n_ - active_bit_level; s--) {
		block_size = pow(2, s);
		branch_size = block_size / 2;
		for (int j = l; j > -1; j -= block_size) {
			//print(j);
			if (j % block_size >= branch_size){
				B_[j - branch_size][s - 1] = B_[j][s] ^ B_[j - branch_size][s];
				B_[j][s - 1] = B_[j][s];
			}
		}
	}
	
}

int PolarCode::active_bit_level(int& i)
{
	int mask = pow(2, (n_ - 1));
	int count = 1;
	for (int k = 0; k < n_; k++) {
		if ((mask & i) > 0) {
			count++;
			mask = mask >> 1;
		}
		else {
			break;
		}
	}
	return std::min(count, n_);
}


void PolarCode::print(std::vector<double>& vec){
	std::cout << std::endl;
	for (int i = 0; i < vec.size(); i++) {
		std::cout << " " << vec[i] << " ";
	}
	std::cout << std::endl;
}

void PolarCode::print(std::vector<int>& vec) {
	std::cout << std::endl;
	for (int i = 0; i < vec.size(); i++) {
		std::cout << " " << vec[i] << " ";
	}
	std::cout << std::endl;
}

void PolarCode::printf(std::vector<int>& vec)
{
	std::cout << std::endl;
	for (int i = 0; i < vec.size(); i++) {
		std::cout << " " << vec[i] << ", ";
	}
	std::cout << std::endl;
}

void PolarCode::print(double x)
{
	std::cout << x << std::endl;
}

void PolarCode::print(int x)
{
	std::cout << x << std::endl;
}

void PolarCode::print(std::vector<std::vector<double>>& vec) {
	std::cout << std::endl;
	for (int i = 0; i < vec.size(); i++) {
		for (int j = 0; j < vec[i].size(); j++) {
			std::cout << vec[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}