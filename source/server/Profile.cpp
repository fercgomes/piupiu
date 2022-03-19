#include "Profile.hpp"
#include <algorithm>
#include <iostream>
#include <mutex>

Profile::Profile(std::string handle, ProfileManager* manager, Session* session)
    : handle(handle), manager(manager), session(session)
{
}

Profile::Profile(std::string handle, ProfileManager* manager) : handle(handle), manager(manager) {}

std::string Profile::GetHandle() const { return handle; }
Session*    Profile::GetSession() const { return session; }

int Profile::AddFollower(std::string userHandle, bool saveDisk)
{
    auto pProfile = manager->GetProfileByName(userHandle);

    if (pProfile)
    {
        // Check if it's already a follower
        auto currentFollowerIt = std::find(followers.begin(), followers.end(), pProfile);
        if (currentFollowerIt == followers.end())
        {
            // Not following
            followers.push_back(pProfile);
            if (saveDisk) manager->Sync();
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        std::cerr << "Usuário não existe" << std::endl;
    }
}

std::vector<Profile*> Profile::GetFollowers() { return followers; }

void Profile::SetSession(Session* session) { this->session = session; }