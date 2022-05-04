#pragma once

#include<string>
#include<unordered_map>
#include<mutex>
#include"profile.hpp"
#include"server.hpp"

// Functions for managing profiles

// sets profile file
void setProfileFile(const std::string &profileFile);

// creates profile manager
void createProfileManager(const std::string &profileFile);

// prints all profiles and their followers
void printProfiles();

// return a pointer to a profile
Profile *getProfile(const std::string &name);

// creates a profile
void createProfile(const std::string &name, bool save = true);

// sends all profile information from user to server
void sendProfileInfo(Server *server);

// loads and saves profiles
void loadProfiles();
void saveProfiles();
void safeSaveProfiles();