#include "../../include/server/profile.hpp"
#include "../../include/server/profilemanager.hpp"
#include <iostream>

const int Profile::MAX_SESSIONS = 2;


Profile::Profile(const std::string &profileName): profileName(profileName), numSessions(0){
    notificationsMutex = new std::mutex;
    sessionsMutex = new std::mutex;
    readMapMutex = new std::mutex;
    followersMutex = new std::mutex;
    notEmpty = new std::condition_variable;
};

Profile::Profile(const Profile &p){
    p.notificationsMutex->lock();
    p.sessionsMutex->lock();
    p.readMapMutex->lock();

    profileName = p.profileName;
    followers = p.followers;
    readMap = p.readMap;
    notifications = p.notifications;
    numSessions = p.numSessions;

    notificationsMutex = p.notificationsMutex;
    sessionsMutex = p.sessionsMutex;
    readMapMutex = p.readMapMutex;
    followersMutex = p.followersMutex;
    notEmpty = p.notEmpty;


    p.readMapMutex->unlock();
    p.sessionsMutex->unlock();
    p.notificationsMutex->unlock();
}

const std::string &Profile::getName() const{
    return profileName;
}
std::vector<std::string> Profile::getFollowers(){
    std::unique_lock<std::mutex> mlock(*followersMutex);
    return followers;
};

void Profile::putNotification(const std::string &message, const std::string &sender){
    std::cout<< "1" << sender << " "<< profileName << " " << message << std::endl;
    notificationsMutex->lock();

    std::cout<< "2" << sender << " "<< profileName << " " << message << std::endl;
    notifications.push(Notification(message, sender));
    std::cout << "NOT LEN:" << notifications.size() << std::endl;
    notEmpty->notify_all();

    notificationsMutex->unlock();
}
Notification Profile::readNotification(const std::string &id){
    std::unique_lock<std::mutex> mlock(*notificationsMutex);

    while(notifications.empty()){
        notEmpty->wait(mlock);
    }

    Notification &notificationRef = notifications.front();
    notificationRef.incrementRead();

    Notification notification = notificationRef;


    sessionsMutex->lock();
    readMapMutex->lock();

    readMap.insert(id);

    std::cout << readMap.size()<< std::endl;
    std::cout << notificationRef.getRead() << std::endl;
    std::cout << numSessions << std::endl;
    if(notificationRef.getRead() == numSessions){
        notifications.pop();
        readMap.clear();
    }   

    readMapMutex->unlock();
    sessionsMutex->unlock();

    return notification;
}

bool Profile::canRead(const std::string &id){
    readMapMutex->lock();

    bool can = (readMap.find(id) == readMap.end());

    readMapMutex->unlock();

    return can;
}

void Profile::addFollower(const std::string &follower, bool save){
    followersMutex->lock();

    followers.push_back(follower);

    followersMutex->unlock();
    if(save)
        safeSaveProfiles();
}

void Profile::notifyFollowers(const std::string &message){
    for (auto fol:getFollowers()){
        getProfile(fol)->putNotification(message, profileName);
    }
}

void Profile::incrementSessions(){
    sessionsMutex->lock();

    ++numSessions;

    sessionsMutex->unlock();
}

void Profile::decrementSessions(){
    sessionsMutex->lock();

    --numSessions;

    sessionsMutex->unlock();
}

int Profile::getSessions(){
    sessionsMutex->lock();

    int val = numSessions;

    sessionsMutex->unlock();

    return numSessions;
}