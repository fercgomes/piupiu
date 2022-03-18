#include "ProfileManager.hpp"
#include <algorithm>
#include <iostream>
#include <mutex>

struct profileHasName
{
    std::string username;

    profileHasName(std::string username) : username(username) {}

    bool operator()(const Profile* profile) const
    {
        return (profile->GetHandle().compare(username) == 0);
    }
};

Profile* ProfileManager::NewProfile(std::string handle, Session* session)
{
    Profile* p = new Profile(handle, this, session);
    {
        const std::lock_guard<std::mutex> lock_guard(profileMutex);
        profiles.push_back(p);
    }
    Sync();

    return p;
}

Profile* ProfileManager::NewProfile(std::string handle)
{
    Profile* p = new Profile(handle, this);
    {
        const std::lock_guard<std::mutex> lock_guard(profileMutex);
        profiles.push_back(p);
    }
    Sync();

    return p;
}

void ProfileManager::Sync()
{
    // TODO
    std::cout << "Syncing profiles to disk" << std::endl;
}

Profile* ProfileManager::GetProfileByName(std::string userHandle)
{
    const std::lock_guard<std::mutex> lock_guard(profileMutex);

    auto it = std::find_if(profiles.begin(), profiles.end(), profileHasName(userHandle));
    if (it != profiles.end()) { return *it; }
    else
    {
        return nullptr;
    }
}