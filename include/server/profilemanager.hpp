#pragma once

#include<string>
#include<unordered_map>
#include<mutex>
#include"profile.hpp"
#include"server.hpp"

// Functions for managing profiles

// creates profile manager
void createProfileManager(const std::string &profileFile);

// prints all profiles and their followers
void printProfiles();

// return a pointer to a profile
Profile *getProfile(const std::string &name);

// creates a profile
void createProfile(const std::string &name, bool save = true);

void sendPendingNotifications(Server *server);

// loads and saves profiles
void loadProfiles();
void saveProfiles();
void safeSaveProfiles();