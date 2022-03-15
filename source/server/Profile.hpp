#pragma once
#include <array>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include "ProfileManager.hpp"
#include "Session.hpp"

class ProfileManager;

class Profile
{
private:
    std::string             handle;
    ProfileManager*         manager;
    std::vector<Profile*>   followers;
    std::array<Session, 2> openSessions;

public:
    Profile(std::string handle, ProfileManager* manager);

    std::string           GetHandle() const;
    std::vector<Profile*> GetFollowers();
    void                  AddFollower(std::string userHandle);
};

using ProfileRef = std::unique_ptr<Profile>&;