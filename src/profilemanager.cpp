#include "../include/profilemanager.hpp"
#include "../include/profile.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include<map>

ProfileManager::ProfileManager(std::string profileFile){
    this->profileFile = profileFile;
    loadProfiles();
}

void ProfileManager::saveProfiles(){
    profileFileMutex.lock();
    std::ofstream outputFile(profileFile, std::ios::trunc);

    for(const auto &any: profiles) {
        const Profile &profile = any.second;

        std::vector<std::string> followers = profile.getFollowersString();
        outputFile << profile.getName() << " ";

        for(const std::string &follower: followers){
            outputFile << follower << " ";
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
    std::string profile;
    while(getline(inputFile, line)) {
        std::stringstream stream(line);

        std::unordered_map <std::string, int> m;
        m["foo"] = 42;
        
        stream >> profile;
        Profile prof = Profile(profile);
        profiles.insert(std::make_pair(profile, prof));
        
        std::string follower;
        while(stream >> follower) {
            prof.addFollower(follower);
        }
    }

}
