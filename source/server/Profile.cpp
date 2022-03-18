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

void Profile::AddFollower(std::string userHandle)
{
    auto pProfile = manager->GetProfileByName(userHandle);
    if (pProfile) { followers.push_back(pProfile); }
    else
    {
        std::cerr << "Usuário não existe" << std::endl;
    }
}

std::vector<Profile*> Profile::GetFollowers() { return followers; }

void Profile::SetSession(Session* session) { this->session = session; }