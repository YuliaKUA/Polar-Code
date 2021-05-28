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

#include <iostream>
#include <fstream>

int main() {

    setlocale(LC_ALL, "rus");

    //Мы определяем основные параметры кода: длину кодового слова и количество информационных битов.
    int n = 8;
    int K = 50;
    
    //создаем экземпляр базового канала
    //BscChannel channel(0.1);
    AwgnBpskChannel channel_awgn(5.0, n, K);
        
    //Затем мы создаем экземпляр кода, используя метод построения, метод построения, DBEC(Dependent Binary Erasure Channel construction),
    //который упорядочивает полярно преобразованные бинарные каналы стирания (BEC) в соответствии с их Z-параметром Бхаттачарьи. 
    
    //PolarCode code(n, K, channel);
    PolarCode code(n, K, channel_awgn);

    //наше сообщение (информационные биты):
    //std::vector<int> message = { 1, 0, 1, 0, 1, 1, 0, 0, 0, 1 };
    std::vector<int> message = { 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
        0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1 };
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
    std::vector<double> transmit_message = channel_awgn.transmit(modulate_message);
    //std::vector<int> demodulate_message = channel_awgn.demodulate(transmit_message);

    //code.receive_likelihoods(channel_awgn.get_likelihoods());

    //std::cout << "Transmit message: ";
    //printd(demodulate_message);
    //print(transmit_message);
    //std::cout << "Is Equal (encode and demodulate): " << ((encode_message == demodulate_message) ? "yes" : "no") << std::endl;

    //Декодируем наше сообщение
    std::cout << "Decode message: ";
    //std::vector<int> decode_message = code.decode(demodulate_message);
    SCdecode decode(code);
    std::vector<int> decode_message = decode.decode(transmit_message);
    print(decode_message);

    //Проверка
    std::cout << "Is Equal(orig message and decode message): " << ((message == decode_message)? "yes" : "no" ) << std::endl;
    
    std::cout << std:: endl << "Info bit position: ";
    print(code.get_info_bit_position());

    std::cout << "Error: " << decode.get_error(message, decode_message) << "\n";

    ////Списочный декодер
    //SCLdecode scl_decode(code);
    //std::vector<int> decode_message = scl_decode.decode(demodulate_message);
    //print(decode_message);



    //Построение графика
    //std::ofstream out/*("C:\\Project\\PolarCode_cpp\\error.txt", std::ios::app)*/;
    //out.open("C:\\Project\\PolarCode_cpp\\error.txt");

    //if (out.is_open())
    //{
    //    out << "x" << "       " << "y\n";
    //}

    //for (double i = 0; i <= 10; i += 0.5) {
    //    
    //    int n = 11;
    //    int K = 1000;

    //    AwgnBpskChannel channel_awgn(i, n, K);

    //    PolarCode code(n, K, channel_awgn);

    //    //std::vector<int> message = { 1, 0, 1, 0, 1, 1, 0, 0, 0, 1 };
    //    std::vector<int> message = { 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1,0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1,0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1,0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1,0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1,0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1,0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1,0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1,0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //                    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0 ,1, 0, 0, 1, 0, 1,
    //                    1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1 };
    //    //std::vector<int> message = { 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
    //    //    0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1 };
    //    
    //  
    //    std::vector<int> encode_message = code.encode(message);

    //    std::vector<int> modulate_message = channel_awgn.modulate(encode_message);
    //    std::vector<double> transmit_message = channel_awgn.transmit(modulate_message);

    //    SCdecode decode(code);
    //    std::vector<int> decode_message = decode.decode(transmit_message);
    //    
    //    if (out.is_open())
    //    {
    //        out << channel_awgn.get_Eb_No() <<  "       " << decode.get_error(message, decode_message) << "\n";
    //        //out << channel_awgn.get_Eb_No() << "       " << channel_awgn.get_ber() << "\n";
    //    }
    //    
    //}

    //out.close();



    system("pause");
    return 0;
}
