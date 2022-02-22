#pragma once
#include "ProfileManager.hpp"
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

class ProfileManager;

class Profile
{
private:
    std::string           handle;
    ProfileManager*       manager;
    std::vector<Profile*> following;
    std::mutex            queueMutex;

public:
    Profile(std::string handle, ProfileManager* manager);

    std::string           GetHandle() const;
    std::vector<Profile*> GetFollowers();
    void                  Follow(std::string userHandle);
};

using ProfileRef = std::unique_ptr<Profile>&;