#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include "Profile.hpp"
#include "Socket.hpp"
// #include "server.hpp"

class Profile;
struct Session;
class Server;

class ProfileManager
{
private:
    Server*               server;
    std::vector<Profile*> profiles;

    std::mutex profileMutex;

    int ReadProfilesFromFile();

public:
    ProfileManager();
    ProfileManager(Server* server);

    Profile*                   GetProfileByName(std::string userHandle);
    Profile*                   NewProfile(std::string handle, Session* session);
    Profile*                   NewProfile(std::string handle);
    void                       Sync();
    std::vector<std::string>   GetConnectedUsers(Profile* exclude = nullptr);
    std::vector<SocketAddress> GetConnectedSockets(Profile* exclude = nullptr);
    std::string                GetProfilesPath();
}; // namespace ProfileManager
