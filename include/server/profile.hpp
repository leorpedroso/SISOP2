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
        std::vector<Profile> followers;
        std::unordered_set<std::thread::id> readMap;
        std::queue<Notification> notifications;
        int numSessions;
        std::shared_ptr<std::mutex> notificationsMutex;
        std::shared_ptr<std::mutex> sessionsMutex;
        std::shared_ptr<std::mutex> readMapMutex;
        std::shared_ptr<std::condition_variable> notEmpty;
        


    public:
        const static int MAX_SESSIONS;

        Profile(const std::string &profileName): profileName(profileName), 
                                                 notificationsMutex(new std::mutex),
                                                 sessionsMutex(new std::mutex),
                                                 readMapMutex(new std::mutex),
                                                 notEmpty(new std::condition_variable) {};

        Profile(const Profile &p): profileName(p.profileName), 
                                   followers(p.followers), 
                                   readMap(p.readMap),
                                   notifications(p.notifications),
                                   numSessions(numSessions),
                                   notificationsMutex(p.notificationsMutex),
                                   sessionsMutex(p.sessionsMutex),
                                   readMapMutex(p.readMapMutex),
                                   notEmpty(p.notEmpty) {};
      
        const std::string &getName() const;
        const std::vector<Profile> &getFollowers() const;

        void putNotification(const Notification &notification);
        Notification readNotification(std::thread::id id);
        bool canRead(std::thread::id id);

        void addFollower(const Profile &follower);

        void incrementSessions();
        void decrementSessions();
        int getSessions();
};