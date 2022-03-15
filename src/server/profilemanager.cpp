#include "../../include/server/profilemanager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// profiles map
std::unordered_map<std::string, Profile*> _profiles;
// profile file
std::string _profileFile;
// mutex for profiles
std::mutex _profileMapMutex;

void createProfileManager(const std::string &profileFile){
    _profileFile = profileFile;
    loadProfiles();
}

void printProfiles(){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);
    for(auto &any: _profiles) {

        std::cout << any.second->getName();

        for(const std::string &follower: any.second->getFollowers()){
            std::cout << " " << follower;
        }

        std::cout << std::endl;
    }
}

// DOESN'T USE MUTEX FOR SAVE
void saveProfiles(){
    std::ofstream outputFile(_profileFile, std::ofstream::out | std::ofstream::trunc);

    for(auto any: _profiles) {

        outputFile << any.second->getName();

        for(const std::string &follower: any.second->getFollowers()){
            outputFile << " " << follower;
        }

        outputFile << "\n";
    }
    
    outputFile.close();
}

// DOESN'T USE MUTEX FOR LOAD
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
        Profile *prof = new Profile(profile);
        
        std::string follower;
        while(stream >> follower) {
            prof->addFollower(follower);
        }

        _profiles.insert(std::make_pair(profile, prof));
    }
}

Profile *getProfile(const std::string &name){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);

    auto pos = _profiles.find(name);
    if(pos == _profiles.end()){
         return nullptr;
    } else {
        return pos->second;
    }
}

void createProfile(const std::string &name){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);

    auto pos = _profiles.find(name);

    if(pos != _profiles.end()){
        return;
    } else {
        _profiles.insert({name, new Profile(name)});
        saveProfiles();
    }
}

// USES MUTEX FOR SAVE
void safeSaveProfiles(){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);
    saveProfiles();
}
