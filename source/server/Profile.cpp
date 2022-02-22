#include "Profile.hpp"
#include <algorithm>
#include <iostream>
#include <mutex>

Profile::Profile(std::string handle, ProfileManager* manager) : handle(handle), manager(manager) {}

std::string Profile::GetHandle() const { return handle; }

void Profile::Follow(std::string userHandle)
{
    auto pProfile = manager->GetProfileByName(userHandle);
    if (pProfile) { following.push_back(pProfile); }
    else
    {
        std::cerr << "Usuário não existe" << std::endl;
    }
}