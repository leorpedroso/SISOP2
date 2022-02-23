#include "../../include/server/profilemanager.hpp"
#include <fstream>
#include <sstream>

ProfileManager::ProfileManager(const std::string &profileFile){
    this->profileFile = profileFile;
    loadProfiles();
}

void ProfileManager::saveProfiles(){
    profileFileMutex.lock();
    std::ofstream outputFile(profileFile, std::ios::trunc);

    for(const auto &any: profiles) {
        const Profile &profile = any.second;

        std::vector<std::string> followers = profile.getFollowersString();
        outputFile << profile.getName();

        for(const std::string &follower: followers){
            outputFile << " " << follower;
        }

        outputFile << "\n";
    }
    
    outputFile.close();
    profileFileMutex.unlock();
}

void ProfileManager::loadProfiles(){
    std::ifstream inputFile(profileFile);

    if (!inputFile.is_open()) {
        return;
    }

    std::string line;
    while(getline(inputFile, line)) {
        std::stringstream stream(line);
        std::string profile;
        
        stream >> profile;
        Profile prof = Profile(profile);
        profiles.insert(std::make_pair(profile, prof));
        
        std::string follower;
        while(stream >> follower) {
            prof.addFollower(follower);
        }
    }
}

std::shared_ptr<Profile> ProfileManager::getProfile(const std::string &name){
    auto pos = profiles.find(name);
    if(pos ==  profiles.end()){
        return nullptr;
    } else {
        return std::make_shared<Profile>(pos->second);
    }
}

void ProfileManager::createProfile(const std::string &name){
    if(getProfile(name) != nullptr){
        throw std::invalid_argument("Name already exists");
    } else {
        Profile prof = Profile(name);
        profiles.insert(std::make_pair(name, prof));
    }
}
