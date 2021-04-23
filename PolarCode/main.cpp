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
    
    //создаем экземпляр базового канала
    //BscChannel channel(0.1);
    AwgnBpskChannel channel_awgn(-1.0, 8, 10);

    //Мы определяем основные параметры кода: длину кодового слова и количество информационных битов.
    //Затем мы создаем экземпляр кода, используя метод построения, метод построения, DBEC(Dependent Binary Erasure Channel construction),
    //который упорядочивает полярно преобразованные бинарные каналы стирания (BEC) в соответствии с их Z-параметром Бхаттачарьи. 
    //Вероятность стирания BEC выбирается таким образом, чтобы начальный канал имел ту же емкость, что и виртуальный BEC
    int n = 8;
    int K = 10;
    //PolarCode code(n, K, channel);
    PolarCode code(n, K, channel_awgn);

    //наше сообщение (информационные биты):
    std::vector<int> message = { 1, 0, 1, 0, 1, 1, 0, 0, 0, 1 };
    //std::vector<int> message = {0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //            0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //            1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1};

    std::cout << "Message:          ";
    print(message);

    //кодируем наше сообщение (применяем к нему полярное преобразование)
    std::vector<int> encode_message = code.encode(message);
    std::cout << "Encode message:   ";
    printd(encode_message);

    //передаем сообщение по каналу
    //модулирем/демодулируем
    std::vector<int> modulate_message = channel_awgn.modulate(encode_message);
    std::vector<double> transmit_message = channel_awgn.transmit(modulate_message, n, K);
    std::vector<int> demodulate_message = channel_awgn.demodulate(transmit_message);

    code.receive_likelihoods(channel_awgn.get_likelihoods());

    std::cout << "Transmit message: ";
    printd(demodulate_message);
    std::cout << "Is Equal (encode and demodulate): " << ((encode_message == demodulate_message) ? "yes" : "no") << std::endl;

    //Декодируем наше сообщение
    std::cout << "Decode message: ";
    //std::vector<int> decode_message = code.decode(demodulate_message);
    SCdecode decode(code);
    std::vector<int> decode_message = decode.decode(demodulate_message);
    print(decode_message);

    //Проверка
    std::cout << "Is Equal(orig message and decode message): " << ((message == decode_message)? "yes" : "no" ) << std::endl;
    

    print(code.get_info_bit_position());

    system("pause");
    return 0;
}
