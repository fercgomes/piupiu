#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include "Profile.hpp"

class Profile;

class ProfileManager
{
private:
    std::vector<Profile*> profiles;

    // Sincroniza os perfis em disco
    void Sync();

public:
    Profile* GetProfileByName(std::string userHandle);

    Profile* NewProfile(std::string handle);
}; // namespace ProfileManager
