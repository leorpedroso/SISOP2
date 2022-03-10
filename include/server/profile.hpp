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
#include<unordered_set>

class Profile{
    private:
        std::string profileName;
        std::unordered_set<std::string> followers;
        std::unordered_set<std::string> readMap;
        std::queue<Notification> notifications;
        int numSessions;
        std::mutex notificationsMutex;
        std::mutex sessionsMutex;
        std::mutex readMapMutex;
        std::mutex followersMutex;
        std::condition_variable notEmpty;
        


    public:
        const static int MAX_SESSIONS;

        Profile(const std::string &profileName): profileName(profileName), numSessions(0){};

        const std::string &getName() const;
        std::unordered_set<std::string> getFollowers();

        void putNotification(const std::string &message, const std::string &sender, const std::string &time);
        Notification readNotification(const std::string &id);
        bool canRead(const std::string &id);

        void addFollower(const std::string &follower, bool save = false);
        void notifyFollowers(const std::string &message, const std::string &time);

        void incrementSessions();
        void decrementSessions();
        int getSessions();
};