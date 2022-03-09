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
    std::ofstream outputFile(_profileFile, std::ofstream::out | std::ofstream::trunc);

    for(auto &any: _profiles) {
        Profile profile = any.second;

        std::vector<std::string> followers = profile.getFollowers();
        outputFile << any.second.getName();

        for(const std::string &follower: followers){
            outputFile << " " << follower;
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
        std::cout << "line:" << line << std::endl;
        std::stringstream stream(line);
        std::string profile;
        
        stream >> profile;
        std::cout << "profile:" << profile << std::endl;
        Profile prof = Profile(profile);
        
        std::string follower;
        while(stream >> follower) {
            std::cout << "follower:" << follower  << std::endl;
            prof.addFollower(follower);
            for(auto algumacoisa:prof.getFollowers()) std::cout << algumacoisa << std::endl;
        }
        for(auto algumacoisa:prof.getFollowers()) std::cout << algumacoisa << std::endl;

        _profiles.insert(std::make_pair(profile, prof));
    }

    for(auto &any: _profiles) {
        Profile profile = any.second;

        std::vector<std::string> followers = profile.getFollowers();
        std::cout << any.second.getName() << std::endl;

        for(const std::string &follower: followers){
            std::cout << " " << follower << std::endl;
        }

        std::cout << "\n" << std::endl;
    }
}

Profile *getProfile(const std::string &name){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);

    std::cout << "given name: " << name << std::endl;
    auto pos = _profiles.find(name);
    if(pos == _profiles.end()){
         return nullptr;
    } else {
        std::cout << "profile name: " << pos->second.getName() << std::endl;
        return &(pos->second);
    }
}

void createProfile(const std::string &name){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);

    auto pos = _profiles.find(name);

    if(pos != _profiles.end()){
        return;
    } else {
        Profile prof(name);
        _profiles.insert({name, prof});
        saveProfiles();
    }
}

void safeSaveProfiles(){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);
    saveProfiles();
}
