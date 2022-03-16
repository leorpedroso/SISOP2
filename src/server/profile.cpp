#include "../../include/server/profile.hpp"
#include "../../include/server/profilemanager.hpp"
#include <iostream>
#include <chrono>

// Constant of maximum number of sessions that a client can have active 
const int Profile::MAX_SESSIONS = 2;

// Returns the profile name of a client
const std::string &Profile::getName() const{
    return profileName;
}

// Returns the current followers of a profile client, uses a mutex to avoid wrong data
std::unordered_set<std::string> Profile::getFollowers(){
    std::unique_lock<std::mutex> mlock(followersMutex);
    return followers;
};

// Sends the profile notification, uses a mutex to avoid miss data on the send
void Profile::putNotification(const std::string &message, const std::string &sender, const std::string &time){
    notificationsMutex.lock();
    notifications.push(Notification(message, sender, time));
    notEmpty.notify_all();
    notificationsMutex.unlock();
}

// Reads notification from a client if it is not empty, uses mutex to avoid miss data
Notification Profile::readNotification(const std::string &id){
    std::unique_lock<std::mutex> mlock(notificationsMutex);

    // Waits for an not empty message
    if (notEmpty.wait_for(mlock, std::chrono::microseconds(2), [this]{return !notifications.empty();}) == false)
        return Notification("", "", "");
    Notification &notificationRef = notifications.front();
    notificationRef.incrementRead();
    Notification notification = notificationRef;

    sessionsMutex.lock();
    readMapMutex.lock();

    // Insert id to read line
    readMap.insert(id);
    if(notificationRef.getRead() == numSessions){
        notifications.pop();
        readMap.clear();
    }   

    readMapMutex.unlock();
    sessionsMutex.unlock();

    return notification;
}

// Check if the id is the next on the line to be read, uses mutex to avoid miss data
bool Profile::canRead(const std::string &id){
    readMapMutex.lock();
    bool can = (readMap.find(id) == readMap.end());
    readMapMutex.unlock();
    return can;
}

// If gets a message asking to follow someone, add a follower on the list for a client
// Uses mutex to avoid miss data
bool Profile::addFollower(const std::string &follower, bool save){
    bool alreadyFollows = true;
    followersMutex.lock();
    if (followers.find(follower) == followers.end()) {
        followers.insert(follower);
        alreadyFollows = false;
    }
    followersMutex.unlock();

    // If it is already on the list, do not save it
    if(save && !alreadyFollows)
        safeSaveProfiles();
    return alreadyFollows;
}

// Sends a message to a profile that other clients are following it
void Profile::notifyFollowers(const std::string &message, const std::string &time){
    for (auto fol:getFollowers()){
        getProfile(fol)->putNotification(message, profileName, time);
    }
}

// Add a client session for a profile, uses mutex to avoid wrong data
void Profile::incrementSessions(){
    sessionsMutex.lock();
    ++numSessions;
    sessionsMutex.unlock();
}

// Remove a client session for a profile, uses mutex to avoid wrong data
void Profile::decrementSessions(){
    sessionsMutex.lock();
    --numSessions;
    sessionsMutex.unlock();
}

// Gets the current number of client sessions that a profile, uses mutex to avoid wrong data
int Profile::getSessions(){
    sessionsMutex.lock();
    int val = numSessions;
    sessionsMutex.unlock();
    return numSessions;
}