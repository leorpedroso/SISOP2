#include "../../include/server/profile.hpp"

const std::string &Profile::getName() const{
    return profileName;
}
const std::vector<std::string> &Profile::getFollowersString() const{
    return followers;
};

void Profile::putNotification(const Notification &notification){
    notificationsMutex->lock();

    notifications.push(notification);
    notEmpty->notify_one();

    notificationsMutex->unlock();
}
Notification Profile::readNotification(){
    std::unique_lock<std::mutex> mlock(*notificationsMutex);

    while(notifications.empty()){
        notEmpty->wait(mlock);
    }

    Notification &notificationRef = notifications.front();
    notificationRef.incrementRead();

    Notification notification = notificationRef;

    sessionsMutex->lock();
    if(notificationRef.getRead() == numSessions){
        notifications.pop();
    }   
    sessionsMutex->unlock();

    return notification;
}

void Profile::addFollower(const std::string &follower){
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