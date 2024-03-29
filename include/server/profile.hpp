#pragma once

#include"notification.hpp"
#include<string>
#include<vector>
#include<mutex>
#include<queue>
#include<unordered_set>
#include<thread>
#include<memory>
#include<condition_variable>
#include"server.hpp"
#include<unordered_set>

class Profile{
    private:
        // profilename and mutex
        std::string profileName;
        std::mutex nameMutex;

        // followers set
        std::unordered_set<std::string> followers;

        // set for sessions that already read the last notification
        std::unordered_set<std::string> readMap;

        // queue for notifications
        std::queue<Notification> notifications;

        // number of sessions
        int numSessions;

        // session addrs
        std::vector<std::string> sessionsAddrs;

        // mutexes
        std::mutex notificationsMutex; // mutex for notification queue
        std::mutex sessionsMutex; // mutex for number of sessions
        std::mutex readMapMutex; // mutex for readMap
        std::mutex followersMutex; // mutex for followers set


        // condition variable that indicates that the notification queue is not empty
        std::condition_variable notEmpty;
        


    public:
        // constant for max number of sessions
        const static int MAX_SESSIONS;

        Profile(const std::string &profileName): profileName(profileName), numSessions(0){};

        // getters for name and followers
        const std::string &getName();
        std::unordered_set<std::string> getFollowers();

        //  puts notification on list
        void putNotification(const std::string &message, const std::string &sender, const std::string &time);
        // pops notification
        void popNotification();
        // reads notification
        Notification readNotification(const std::string &id);
        // checks if can read a notification using a thread id
        bool canRead(const std::string &id);

        // add a follower
        bool addFollower(const std::string &follower, bool save = false);
        // adds a notification from this profile to its followers
        void notifyFollowers(const std::string &message, const std::string &time);
        // sends all required information to server
        void sendAllInfo(Server *server);

        // functions for changing number of sessions
        void incrementSessions(const std::string &sess);
        void decrementSessions(const std::string &sess);
        int getSessions();
};