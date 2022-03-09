#include "../../include/server/profilemanager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

std::unordered_map<std::string, Profile> _profiles;
std::string _profileFile;
std::mutex _profileFileMutex;
std::mutex _profileMapMutex;

void createProfileManager(const std::string &profileFile){
    _profileFile = profileFile;
    loadProfiles();
}


void saveProfiles(){
    std::ofstream outputFile(_profileFile, std::ios::trunc);

    for(const auto &any: _profiles) {
        const Profile &profile = any.second;

        std::vector<Profile> followers = profile.getFollowers();
        outputFile << profile.getName();
        std::cout << followers.size() << std::endl;
        for(const Profile &follower: followers){
            outputFile << " " << follower.getName();
        }

        outputFile << "\n";
    }
    
    outputFile.close();
}

void loadProfiles(){
    std::ifstream inputFile(_profileFile);

    if (!inputFile.is_open()) {
        return;
    }

    std::string line;
    while(getline(inputFile, line)) {
        std::stringstream stream(line);
        std::string profile;
        
        stream >> profile;
        std::shared_ptr<Profile> profPtr = getProfile(profile);
        Profile prof = Profile(profile);
        if (profPtr != nullptr){
            prof = *profPtr;
        }

        _profiles.insert(std::make_pair(profile, prof));
        
        std::string follower;
        while(stream >> follower) {
            std::shared_ptr<Profile> folPtr = getProfile(follower);
            Profile profFol = Profile(follower);

            if (folPtr != nullptr){
                profFol = *folPtr;
            }

            prof.addFollower(profFol);
        }
    }
}

std::shared_ptr<Profile> getProfile(const std::string &name){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);

    auto pos = _profiles.find(name);
    if(pos == _profiles.end()){
         return nullptr;
    } else {
        return std::make_shared<Profile>(pos->second);
    }
}

void createProfile(const std::string &name){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);

    auto pos = _profiles.find(name);

    if(pos != _profiles.end()){
        return;
    } else {
        Profile prof = Profile(name);
        _profiles.insert(std::make_pair(name, prof));
        saveProfiles();
    }
}

void safeSaveProfiles(){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);
    saveProfiles();
}
