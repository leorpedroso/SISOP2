#include "../../include/server/profile.hpp"
#include "../../include/server/profilemanager.hpp"
#include "../../include/server/servermanager.hpp"
#include <iostream>
#include <chrono>
#include<algorithm>

// Constant of maximum number of sessions that a client can have active 
const int Profile::MAX_SESSIONS = 2;

// Returns the profile name
const std::string &Profile::getName(){
    std::unique_lock<std::mutex> mlock(nameMutex);
    return profileName;
}

// Returns the current profile followers. Uses a mutex to avoid problems, as this data can be accessed/modified by multiple threads
std::unordered_set<std::string> Profile::getFollowers(){
    std::unique_lock<std::mutex> mlock(followersMutex);
    return followers;
};

// adds the profile notification, uses a mutex to avoid sending corrupted data or sending to wrong subscribers
void Profile::putNotification(const std::string &message, const std::string &sender, const std::string &time){
    notificationsMutex.lock();

    int id = getGlobalMessageCount();
    // -1 forces the notification to be blocked until client is connected and the message will be send with readNotification
    std::shared_ptr<Counter> count(std::make_shared<Counter>(-1));

    addCounterToMap(id, count);

    notifications.push(Notification(message, sender, time, id, count));
    notEmpty.notify_all();
    notificationsMutex.unlock();
}

void Profile::sendAllInfo(Server *server){
    sessionsMutex.lock();

    for(const std::string sess: sessionsAddrs){
        int temp_id = getGlobalMessageCount();
        server->addMsg(Message(Socket::CONNECT_OK, std::to_string(temp_id) + " " + getName() + " " + sess));
    }

    sessionsMutex.unlock();

    notificationsMutex.lock();
    std::queue<Notification> tempNotifications = notifications;
    
    while(!tempNotifications.empty()){
        Notification &notificationRef = tempNotifications.front();
        server->addMsg(Message(Socket::ACK, std::to_string(notificationRef.getID()) + " " + getName() + " ADD_NOT " + notificationRef.getTime() + " " + notificationRef.getSender() + " " + notificationRef.getMessage()));
        tempNotifications.pop();
    }
    notificationsMutex.unlock();
}

void Profile::popNotification(){
    std::unique_lock<std::mutex> mlock(notificationsMutex);
    notifications.pop();
}

// Reads notification from a client if it is not empty, uses mutex to avoid miss data
Notification Profile::readNotification(const std::string &id){
    std::unique_lock<std::mutex> mlock(notificationsMutex);

    // Checks if notification buffer is empty. If it is, blocks the thread until a notify or a timeout. In case of a timeout, returns an empty notification, otherwise it continues the notification reading process.
    if (notEmpty.wait_for(mlock, std::chrono::microseconds(2), [this]{return !notifications.empty();}) == false)
        return Notification("", "", "");
    Notification &notificationRef = notifications.front();

    if(notificationRef.getCount()->getValue() == -1){
        // message will finally be sent, can be considered sent on the backup servers
        notificationRef.getCount()->setValue(getNumberServers());
        addMessagetoServers(Message(Socket::NOTIFICATION, std::to_string(notificationRef.getID()) + " " + getName() + " " + notificationRef.getMessage()));
    }

    if(notificationRef.getCount()->getValue() != 0){
        return Notification("", "", "");
    }

    notificationRef.incrementRead();
    Notification notification = notificationRef;

    sessionsMutex.lock();
    readMapMutex.lock();

    // Insert id to read map
    readMap.insert(id);
    if(notificationRef.getRead() == numSessions){
        notifications.pop();
        readMap.clear();
    }   

    readMapMutex.unlock();
    sessionsMutex.unlock();

    return notification;
}

// Check if the id has already read the notification, uses mutex to avoid miss data
bool Profile::canRead(const std::string &id){
    readMapMutex.lock();
    bool can = (readMap.find(id) == readMap.end());
    readMapMutex.unlock();
    return can;
}

// Adds the given follower the profile's follower list
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

// Sends received notification to the profile's followers
void Profile::notifyFollowers(const std::string &message, const std::string &time){
    for (auto fol:getFollowers()){
        getProfile(fol)->putNotification(message, getName(), time);
    }
}

// Increases the profile's open sessions counter, uses mutex to avoid wrong data
void Profile::incrementSessions(const std::string &sess){
    sessionsMutex.lock();
    sessionsAddrs.push_back(sess);
    ++numSessions;
    sessionsMutex.unlock();
}

// Decreases the profile's open sessions counter, uses mutex to avoid wrong data
void Profile::decrementSessions(const std::string &sess){
    sessionsMutex.lock();
    sessionsAddrs.erase(std::remove(sessionsAddrs.begin(), sessionsAddrs.end(), sess), sessionsAddrs.end());
    --numSessions;
    sessionsMutex.unlock();
}

// Gets the current number of client sessions that the profile has, uses mutex to avoid wrong data
int Profile::getSessions(){
    sessionsMutex.lock();
    int val = numSessions;
    sessionsMutex.unlock();
    return numSessions;
}