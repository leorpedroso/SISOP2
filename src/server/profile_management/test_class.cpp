
#include <iostream>
#include <string>
#include <unistd.h>
#include <time.h>
#include <cstdlib>
#include <thread>
#include <chrono>

#include "../../include/server/test_class.hpp"

#define SEND_WAIT 8
#define RECEIVE_WAIT 15

Test::Test(std::string clientName) {
    srand(time(NULL));
    counter = 0;
    this->clientName = clientName;
}

bool Test::send(std::string msg) {
    std::this_thread::sleep_for (std::chrono::seconds(rand() % SEND_WAIT));
    std::cout << "Client --> Received message: " << msg << std::endl;
    return true;
} 

std::string Test::receive(int timeout) {
    std::string msg = "";
    int sleep_time = rand() % RECEIVE_WAIT;
    if (sleep_time > timeout) {
        std::this_thread::sleep_for (std::chrono::seconds(timeout));
    } else {
        std::this_thread::sleep_for (std::chrono::seconds(sleep_time));
        msg = clientName + " msg " + std::to_string(counter);
        counter++;
        std::cout << "Client --> Sending message: " << msg << std::endl;
    }
    return msg;
}