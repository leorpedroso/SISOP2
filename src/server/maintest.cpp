#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

#include "../../include/server/profile_management/session_manager.hpp"
#include "../../include/server/profile_management/connection_manager.hpp"

using namespace std;

int main() {
    mutex send_mtx;
    SessionManager session(&send_mtx);
    ConnectionManager connection; // TODO implement connection handling
    bool sent = false;
    
    thread listen_thread(&SessionManager::listen, &session);
    listen_thread.detach();

    for(int i = 0; i < 20; i++) {
        string msg_send = "message " + std::to_string(i);

        thread send_thread (&SessionManager::send, &session, msg_send, &sent);

        string msg_receive = session.read_buffer();
        if(msg_receive.compare("") != 0) {
            cout << "received: " << msg_receive << endl;
        }

        send_mtx.lock();
        if (sent) {
            cout << "message sent!" << endl;
            sent = false;
        }
        send_mtx.unlock();

        send_thread.join();
    }

    std::this_thread::sleep_for (std::chrono::seconds(30));
    session.close();
    while(1) {
        cout << "Infinite loop" << endl;
        std::this_thread::sleep_for (std::chrono::seconds(2));
    }
    return 0;
}