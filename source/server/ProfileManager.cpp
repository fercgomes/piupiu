#include "ProfileManager.hpp"
#include <iostream>

Profile::Profile(std::string handle) : handle(handle) {}

std::vector<Profile> ProfileManager::GetProfiles() { return profiles; }

Profile ProfileManager::NewProfile(std::string handle)
{
    Profile p(handle);

    profiles.push_back(p);
    Sync();

    return p;
}

void ProfileManager::Sync() { std::cout << "Syncing profiles to disk" << std::endl; }