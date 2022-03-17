#include "../../include/server/profilemanager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// Profiles map
std::unordered_map<std::string, Profile*> _profiles;
// Profile file
std::string _profileFile;
// Mutex for profiles map
std::mutex _profileMapMutex;

// Init Manager for the profiles and gets all the current profiles
void createProfileManager(const std::string &profileFile){
    _profileFile = profileFile;
    loadProfiles();
}

// Print on the server screen all the profiles saved and they followers
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

// Save the current profiles into the profile file
// Doen't need to use a mutex for saving
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

// Loads all the profiles on the profile file into the server memory
// Doen't need to use a mutex for loading
void loadProfiles(){
    std::ifstream inputFile(_profileFile);
    // If the file ins't open, it can't be read
    if (!inputFile.is_open()) {
        return;
    }

    // Treats all lines from the file to get the data
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

// Get the profile using the name, uses mutex to avoid wrong data
Profile *getProfile(const std::string &name){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);
    auto pos = _profiles.find(name);
    if(pos == _profiles.end()){
         return nullptr;
    } else {
        return pos->second;
    }
}

// Creates a new profile based on the name of the client, uses a mutex to avoid wrong data
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

// Calls the save method using a mutex to avoid wrong data
void safeSaveProfiles(){
    std::unique_lock<std::mutex> mlock(_profileMapMutex);
    saveProfiles();
}
