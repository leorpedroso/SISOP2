#pragma once

#include"notification.hpp"
#include<string>
#include<vector>
#include<mutex>
#include<queue>

class Profile{
    private:
        std::vector<std::string> followers;
        std::string profileName;
        std::queue<Notification> notifications;


    public:
        Profile(std::string profileName): profileName(profileName) {};
        Profile(const Profile &p): profileName(p.profileName), 
                                   followers(p.followers), 
                                   notifications(p.notifications) {};
      
        const std::string &getName() const;
        const std::vector<std::string> &getFollowersString() const;

        void putNotification(Notification notification);
        Notification readNotification();
        void addFollower(std::string follower);
};