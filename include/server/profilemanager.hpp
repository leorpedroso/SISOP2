#pragma once

#include<string>
#include<unordered_map>
#include<mutex>
#include"profile.hpp"

class ProfileManager{
    private:
        std::unordered_map<std::string, Profile> profiles;
        std::string profileFile;
        std::mutex profileFileMutex;

        void loadProfiles();

    public:
        ProfileManager(const std::string &profileFile);
        
        void saveProfiles();

        std::shared_ptr<Profile> getProfile(const std::string &name);

        void createProfile(const std::string &name);
};