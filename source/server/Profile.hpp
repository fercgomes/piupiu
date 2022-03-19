#pragma once
#include <array>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include "ProfileManager.hpp"

class ProfileManager;
struct Session;

class Profile
{
private:
    std::string           handle;
    ProfileManager*       manager;
    std::vector<Profile*> followers;
    Session*              session;

public:
    Profile(std::string handle, ProfileManager* manager, Session* session);
    Profile(std::string handle, ProfileManager* manager);

    std::string           GetHandle() const;
    std::vector<Profile*> GetFollowers();
    Session*              GetSession() const;
    int                   AddFollower(std::string userHandle, bool saveDisk = true);
    void                  SetSession(Session* session);
};

using ProfileRef = std::unique_ptr<Profile>&;