#include "../include/profile.hpp"

const std::string &Profile::getName() const{
    return profileName;
}
const std::vector<std::string> &Profile::getFollowersString() const{
    return followers;
};

void Profile::putNotification(Notification notification){
    // TODO IMPLEMENT
}
Notification Profile::readNotification(){
    // TODO IMPLEMENT
}
void Profile::addFollower(std::string follower){
    followers.push_back(follower);
}