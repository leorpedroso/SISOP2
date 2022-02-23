#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

#include "../../include/server/profile_management/session_manager.hpp"
#include "../../include/server/profile_management/connection_manager.hpp"

using namespace std;

int main() {
    SessionManager session;
    ConnectionManager connection; // TODO implement connection handling

    for(int i = 0; i < 20; i++) {
        string msg_send = "message " + std::to_string(i);
        cout << "Server --> Sending message: " << msg_send << endl;
        session.send(msg_send);
        while(!session.msg_sent());
        cout << "Server --> Message sent!" << endl;

        string msg_receive = session.read_buffer();
        if(msg_receive.compare("") != 0) {
            cout << "Server --> Received message: " << msg_receive << endl;
        }
    }

    std::this_thread::sleep_for (std::chrono::seconds(30));
    session.close();
    while(1) {
        cout << "Server --> Infinite loop" << endl;
        std::this_thread::sleep_for (std::chrono::seconds(2));
    }
    return 0;
}