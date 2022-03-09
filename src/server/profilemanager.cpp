#include "../../include/server/profilemanager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

ProfileManager::ProfileManager(const std::string &profileFile): profileFileMutex(new std::mutex), profileMapMutex(new std::mutex){
    this->profileFile = profileFile;
    loadProfiles();
}

void ProfileManager::saveProfiles(){
    std::ofstream outputFile(profileFile, std::ios::trunc);

    for(const auto &any: profiles) {
        const Profile &profile = any.second;

        std::vector<Profile> followers = profile.getFollowers();
        outputFile << profile.getName();

        for(const Profile &follower: followers){
            outputFile << " " << follower.getName();
        }

        outputFile << "\n";
    }
    
    outputFile.close();
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
        std::shared_ptr<Profile> profPtr = getProfile(profile);
        Profile prof = Profile(profile);
        if (profPtr != nullptr){
            prof = *profPtr;
        }

        profiles.insert(std::make_pair(profile, prof));
        
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

std::shared_ptr<Profile> ProfileManager::getProfile(const std::string &name){
    std::unique_lock<std::mutex> mlock(*profileMapMutex);

    auto pos = profiles.find(name);
    if(pos == profiles.end()){
         return nullptr;
    } else {
        return std::make_shared<Profile>(pos->second);
    }
}

void ProfileManager::createProfile(const std::string &name){
    std::unique_lock<std::mutex> mlock(*profileMapMutex);

    auto pos = profiles.find(name);

    if(pos != profiles.end()){
        return;
    } else {
        Profile prof = Profile(name);
        profiles.insert(std::make_pair(name, prof));
        saveProfiles();
    }
}
