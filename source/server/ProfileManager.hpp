#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include "Profile.hpp"

class Profile;
struct Session;

class ProfileManager
{
private:
    std::vector<Profile*> profiles;

    std::mutex profileMutex;

    int ReadProfilesFromFile();

public:
    ProfileManager();

    Profile*                        GetProfileByName(std::string userHandle);
    Profile*                        NewProfile(std::string handle, Session* session);
    Profile*                        NewProfile(std::string handle);
    void                            Sync();
    std::vector<std::string>        GetConnectedUsers(Profile* exclude = nullptr);
    std::vector<struct sockaddr_in> GetConnectedSockets(Profile* exclude = nullptr);
}; // namespace ProfileManager
