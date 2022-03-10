#include "../../include/server/profile.hpp"
#include "../../include/server/profilemanager.hpp"
#include <iostream>
#include <chrono>

const int Profile::MAX_SESSIONS = 2;


const std::string &Profile::getName() const{
    return profileName;
}
std::unordered_set<std::string> Profile::getFollowers(){
    std::unique_lock<std::mutex> mlock(followersMutex);
    return followers;
};

void Profile::putNotification(const std::string &message, const std::string &sender, const std::string &time){
    notificationsMutex.lock();

    notifications.push(Notification(message, sender, time));
    notEmpty.notify_all();

    notificationsMutex.unlock();
}
Notification Profile::readNotification(const std::string &id){
    std::unique_lock<std::mutex> mlock(notificationsMutex);


    if (notEmpty.wait_for(mlock, std::chrono::microseconds(2), [this]{return !notifications.empty();}) == false)
        return Notification("", "", "");

    Notification &notificationRef = notifications.front();
    notificationRef.incrementRead();

    Notification notification = notificationRef;


    sessionsMutex.lock();
    readMapMutex.lock();

    readMap.insert(id);

    if(notificationRef.getRead() == numSessions){
        notifications.pop();
        readMap.clear();
    }   

    readMapMutex.unlock();
    sessionsMutex.unlock();

    return notification;
}

bool Profile::canRead(const std::string &id){
    readMapMutex.lock();

    bool can = (readMap.find(id) == readMap.end());

    readMapMutex.unlock();

    return can;
}

void Profile::addFollower(const std::string &follower, bool save){
    followersMutex.lock();

    followers.insert(follower);

    followersMutex.unlock();
    if(save)
        safeSaveProfiles();
}

void Profile::notifyFollowers(const std::string &message, const std::string &time){
    for (auto fol:getFollowers()){
        getProfile(fol)->putNotification(message, profileName, time);
    }
}

void Profile::incrementSessions(){
    sessionsMutex.lock();

    ++numSessions;

    sessionsMutex.unlock();
}

void Profile::decrementSessions(){
    sessionsMutex.lock();

    --numSessions;

    sessionsMutex.unlock();
}

int Profile::getSessions(){
    sessionsMutex.lock();

    int val = numSessions;

    sessionsMutex.unlock();

    return numSessions;
}