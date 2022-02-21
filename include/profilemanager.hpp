#pragma once

#include<string>
#include<unordered_map>
#include<mutex>

class ProfileManager{
    private:
        std::unordered_map<std::string, Profile> profiles;
        std::string profileFile;
        std::mutex profileFileMutex;

        void loadProfiles();

    public:
        ProfileManager(std::string profileFile);
        
        void saveProfiles();
};