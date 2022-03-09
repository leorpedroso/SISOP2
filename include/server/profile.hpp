#pragma once

#include"notification.hpp"
#include<string>
#include<vector>
#include<mutex>
#include<queue>
#include<thread>
#include<memory>
#include<condition_variable>
#include<unordered_set>

class Profile{
    private:
        std::string profileName;
        std::vector<std::string> followers;
        std::unordered_set<std::thread::id> readMap;
        std::queue<Notification> notifications;
        int numSessions;
        std::mutex *notificationsMutex;
        std::mutex *sessionsMutex;
        std::mutex *readMapMutex;
        std::mutex *followersMutex;
        std::condition_variable *notEmpty;
        


    public:
        const static int MAX_SESSIONS;

        Profile(const std::string &profileName);
        Profile(const Profile &p);

        const std::string &getName() const;
        std::vector<std::string> getFollowers();

        void putNotification(const std::string &message, const std::string &sender);
        Notification readNotification(std::thread::id id);
        bool canRead(std::thread::id id);

        void addFollower(const std::string &follower, bool save = false);
        void notifyFollowers(const std::string &message);

        void incrementSessions();
        void decrementSessions();
        int getSessions();
};