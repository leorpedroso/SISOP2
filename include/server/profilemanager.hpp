#pragma once

#include<string>
#include<unordered_map>
#include<mutex>
#include"profile.hpp"

class ProfileManager{
    private:
        std::unordered_map<std::string, Profile> profiles;
        std::string profileFile;
        std::shared_ptr<std::mutex> profileFileMutex;
        std::shared_ptr<std::mutex> profileMapMutex;

        void loadProfiles();
        void saveProfiles();

    public:
        ProfileManager(const std::string &profileFile);

        
        ProfileManager(ProfileManager &man): profiles(man.profiles),
                                            profileFile(man.profileFile),
                                            profileFileMutex(man.profileFileMutex),
                                            profileMapMutex(man.profileMapMutex){}
        

        std::shared_ptr<Profile> getProfile(const std::string &name);

        void createProfile(const std::string &name);
};