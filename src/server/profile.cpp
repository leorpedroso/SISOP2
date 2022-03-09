#include "../../include/server/profile.hpp"

const int Profile::MAX_SESSIONS = 2;

const std::string &Profile::getName() const{
    return profileName;
}
const std::vector<Profile> &Profile::getFollowers() const{
    return followers;
};

void Profile::putNotification(const Notification &notification){
    notificationsMutex->lock();

    notifications.push(notification);
    notEmpty->notify_all();

    notificationsMutex->unlock();
}
Notification Profile::readNotification(std::thread::id id){
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

    if(notificationRef.getRead() == numSessions){
        notifications.pop();
        readMap.clear();
    }   

    readMapMutex->unlock();
    sessionsMutex->unlock();

    return notification;
}

bool Profile::canRead(std::thread::id id){
    readMapMutex->lock();

    bool can = (readMap.find(id) != readMap.end());

    readMapMutex->unlock();

    return can;
}

void Profile::addFollower(const Profile &follower){
    followers.push_back(follower);
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