#include "ProfileManager.hpp"
#include <algorithm>
#include <iostream>
#include <mutex>

Profile::Profile(std::string handle, ProfileManager* manager) : handle(handle), manager(manager) {}

void Profile::Follow(std::string userHandle)
{
    auto pProfile = manager->GetProfileByName(userHandle);
    if (pProfile) { following.push_back(pProfile); }
    else
    {
        std::cerr << "Usuário não existe" << std::endl;
    }
}

int Profile::PushNotification(Notification notification)
{
    const std::lock_guard<std::mutex> lock(queueMutex);
    pendingNotifications.push(notification);
}

std::vector<Profile*> ProfileManager::GetProfiles() { return profiles; }

Profile* ProfileManager::NewProfile(std::string handle)
{
    Profile* p = new Profile(handle, this);

    profiles.push_back(p);
    Sync();

    return p;
}

void ProfileManager::Sync() { std::cout << "Syncing profiles to disk" << std::endl; }

Profile* ProfileManager::GetProfileByName(std::string userHandle)
{
    auto it = std::find(profiles.begin(), profiles.end(), userHandle);
    if (it != profiles.end()) { return *it; }
    else
    {
        return nullptr;
    }
}