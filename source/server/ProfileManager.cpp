#include "ProfileManager.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include "Session.hpp"
#include "server.hpp"

struct profileHasName
{
    std::string username;

    profileHasName(std::string username) : username(username) {}

    bool operator()(const Profile* profile) const
    {
        return (profile->GetHandle().compare(username) == 0);
    }
};

std::string ProfileManager::GetProfilesPath()
{
    std::stringstream ss;
    std::cout << server << std::endl;
    auto        replicaManager = server->GetReplicaManager();
    std::string primary        = replicaManager->IsPrimary() ? "primary" : "secondary";
    std::string bindIp         = server->GetIpAddr();
    int         bindPort       = server->GetPort();
    ss << "profiles_" << bindIp << ":" << bindPort;
    std::string output = ss.str();
    return output;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> splits;
    std::string              split;
    std::istringstream       ss(s);
    while (std::getline(ss, split, delimiter))
    {
        splits.push_back(split);
    }
    return splits;
}

int ProfileManager::ReadProfilesFromFile()
{
    std::fstream                    profilesFile;
    std::map<std::string, Profile*> profilesMap;
    std::string                     filename = GetProfilesPath();
    std::cout << "Opening " << filename << std::endl;
    profilesFile.open(filename, std::ios::in);

    if (profilesFile)
    {
        int numProfiles;
        profilesFile >> numProfiles;

        // Read profiles
        std::string profileName;
        for (int i = 0; i < numProfiles; i++)
        {
            profilesFile >> profileName;
            Profile* prof = new Profile(profileName, this);
            profilesMap.insert(std::pair<std::string, Profile*>(profileName, prof));
            profiles.push_back(prof);
        }

        std::string line;
        while (!profilesFile.eof())
        {
            profilesFile >> line;
            if (line.empty()) break;

            auto        splits   = split(line, ',');
            std::string user     = splits[0];
            std::string toFollow = splits[1];

            Profile* userPtr = profilesMap.at(user);
            // Profile* toFollowPtr = profiles.at(toFollow);

            // Checks??
            userPtr->AddFollower(toFollow, false);
        }

        std::cout << "Read " << numProfiles << " profiles from disk." << std::endl;
        return numProfiles;
    }
    else
    {
        std::cerr << "Could not open file" << std::endl;
        return 0;
    }
}

ProfileManager::ProfileManager() { int r = ReadProfilesFromFile(); }

ProfileManager::ProfileManager(Server* server) : server(server) { ReadProfilesFromFile(); }

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
    std::cout << "Syncing profiles to disk" << std::endl;
    std::fstream profilesFile;
    std::string  filename = GetProfilesPath();
    profilesFile.open(filename, std::ios::out);
    if (profilesFile)
    {
        // write user names
        profilesFile << profiles.size() << std::endl;
        for (auto profile : profiles)
        {
            profilesFile << profile->GetHandle() << std::endl;
        }

        // write dependencies
        for (auto profile : profiles)
        {
            for (auto follower : profile->GetFollowers())
            {
                profilesFile << profile->GetHandle() << "," << follower->GetHandle() << std::endl;
            }
        }

        profilesFile.close();
    }
    else
    {
        std::cerr << "Could not open file" << std::endl;
    }
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

std::vector<std::string> ProfileManager::GetConnectedUsers(Profile* exclude)
{
    std::vector<std::string> temp;
    for (auto user : profiles)
    {
        if (exclude && user != exclude)
        {
            Session* session = user->GetSession();
            if (session && session->sockets.size() > 0) { temp.push_back(user->GetHandle()); }
        }
    }

    return temp;
}

std::vector<SocketAddress> ProfileManager::GetConnectedSockets(Profile* exclude)
{
    std::vector<SocketAddress> temp;
    for (auto user : profiles)
    {
        if (exclude && user != exclude)
        {
            Session* session = user->GetSession();
            if (session && session->sockets.size() > 0)
            {
                for (auto socket : session->sockets)
                {
                    temp.push_back(socket);
                }
            }
        }
    }

    return temp;
}