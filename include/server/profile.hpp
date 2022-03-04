#pragma once

#include"notification.hpp"
#include<string>
#include<vector>
#include<mutex>
#include<queue>
#include<memory>
#include<condition_variable>

class Profile{
    private:
        std::string profileName;
        std::vector<std::string> followers;
        std::queue<Notification> notifications;
        int numSessions;
        std::shared_ptr<std::mutex> notificationsMutex;
        std::shared_ptr<std::mutex> sessionsMutex;
        std::shared_ptr<std::condition_variable> notEmpty;


    public:
        Profile(const std::string &profileName): profileName(profileName), 
                                                 notificationsMutex(new std::mutex),
                                                 sessionsMutex(new std::mutex),
                                                 notEmpty(new std::condition_variable) {};

        Profile(const Profile &p): profileName(p.profileName), 
                                   followers(p.followers), 
                                   notifications(p.notifications),
                                   numSessions(numSessions),
                                   notificationsMutex(p.notificationsMutex),
                                   sessionsMutex(p.sessionsMutex),
                                   notEmpty(p.notEmpty) {};
      
        const std::string &getName() const;
        const std::vector<std::string> &getFollowersString() const;

        void putNotification(const Notification &notification);
        Notification readNotification();

        void addFollower(const std::string &follower);

        void incrementSessions();
        void decrementSessions();
};