#pragma once

#include<string>
#include<unordered_map>
#include<mutex>
#include"profile.hpp"

void createProfileManager(const std::string &profileFile);

std::shared_ptr<Profile> getProfile(const std::string &name);

void createProfile(const std::string &name);

void loadProfiles();
void saveProfiles();
void safeSaveProfiles();