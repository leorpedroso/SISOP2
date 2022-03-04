#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

#include "../../include/server/profile_management/session_manager.hpp"
#include "../../include/server/profile_management/message_listener.hpp"

using namespace std;

void read_buffer(MessageListener &listener) {
    string msg_receive = listener.read_buffer();
    if(msg_receive.compare("") != 0) {
        cout << "Server --> Received message: " << msg_receive << endl;
    }
}

int main() {
    SessionManager session("client 1");
    MessageListener connectionHandler("new client"); 

    for(int i = 0; i < 20; i++) {
        string msg_send = "message " + std::to_string(i);
        cout << "Server --> Sending message: " << msg_send << endl;
        session.send(msg_send);
        while(!session.msg_sent());
        cout << "Server --> Message sent!" << endl;
        read_buffer(connectionHandler);
        read_buffer(session);
    }

    for (int i = 0; i < 1500000; i++) {
        read_buffer(connectionHandler);
        read_buffer(session);
    }
    session.close();
    connectionHandler.close();
    while(1) {
        cout << "Server --> Infinite loop" << endl;
        std::this_thread::sleep_for (std::chrono::seconds(2));
    }
    return 0;
}