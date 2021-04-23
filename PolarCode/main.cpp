/* Author: Yulia Kuznetsova (YuliaKUA)
 * Date: 05.04.2021
 *
 * See LICENSE */

#include <iostream>

#include "main.h"
#include "bsc_channel.h"
#include "awgn_bpsk_channel.h"
#include "polar_code.h"
#include "sc_decode.h"

#include <vector>



int main() {

    setlocale(LC_ALL, "rus");
    
    //������� ��������� �������� ������
    //BscChannel channel(0.1);
    AwgnBpskChannel channel_awgn(-1.0, 8, 10);

    //�� ���������� �������� ��������� ����: ����� �������� ����� � ���������� �������������� �����.
    //����� �� ������� ��������� ����, ��������� ����� ����������, ����� ����������, DBEC(Dependent Binary Erasure Channel construction),
    //������� ������������� ������� ��������������� �������� ������ �������� (BEC) � ������������ � �� Z-���������� �����������. 
    //����������� �������� BEC ���������� ����� �������, ����� ��������� ����� ���� �� �� �������, ��� � ����������� BEC
    int n = 8;
    int K = 10;
    //PolarCode code(n, K, channel);
    PolarCode code(n, K, channel_awgn);

    //���� ��������� (�������������� ����):
    std::vector<int> message = { 1, 0, 1, 0, 1, 1, 0, 0, 0, 1 };
    //std::vector<int> message = {0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //            0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //            1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1};

    std::cout << "Message:          ";
    print(message);

    //�������� ���� ��������� (��������� � ���� �������� ��������������)
    std::vector<int> encode_message = code.encode(message);
    std::cout << "Encode message:   ";
    printd(encode_message);

    //�������� ��������� �� ������
    //���������/������������
    std::vector<int> modulate_message = channel_awgn.modulate(encode_message);
    std::vector<double> transmit_message = channel_awgn.transmit(modulate_message, n, K);
    std::vector<int> demodulate_message = channel_awgn.demodulate(transmit_message);

    code.receive_likelihoods(channel_awgn.get_likelihoods());

    std::cout << "Transmit message: ";
    printd(demodulate_message);
    std::cout << "Is Equal (encode and demodulate): " << ((encode_message == demodulate_message) ? "yes" : "no") << std::endl;

    //���������� ���� ���������
    std::cout << "Decode message: ";
    //std::vector<int> decode_message = code.decode(demodulate_message);
    SCdecode decode(code);
    std::vector<int> decode_message = decode.decode(demodulate_message);
    print(decode_message);

    //��������
    std::cout << "Is Equal(orig message and decode message): " << ((message == decode_message)? "yes" : "no" ) << std::endl;
    

    print(code.get_info_bit_position());

    system("pause");
    return 0;
}
