#include "ProfileManager.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>

#define PROFILES_FILENAME "profiles.dat"

struct profileHasName
{
    std::string username;

    profileHasName(std::string username) : username(username) {}

    bool operator()(const Profile* profile) const
    {
        return (profile->GetHandle().compare(username) == 0);
    }
};

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
    std::cout << "Reading profiles from file" << std::endl;

    profilesFile.open(PROFILES_FILENAME, std::ios::in);

    if (profilesFile)
    {
        int numProfiles;
        profilesFile >> numProfiles;
        std::cout << numProfiles << std::endl;

        // Read profiles
        std::string profileName;
        for (int i = 0; i < numProfiles; i++)
        {
            profilesFile >> profileName;
            std::cout << profileName << std::endl;
            Profile* prof = new Profile(profileName, this);
            profilesMap.insert(std::pair<std::string, Profile*>(profileName, prof));
            profiles.push_back(prof);
        }

        std::string line;
        while (profilesFile.good())
        {
            profilesFile >> line;
            std::cout << line << std::endl;
            auto        splits   = split(line, ',');
            std::string user     = splits[0];
            std::string toFollow = splits[1];
            std::cout << user << " " << toFollow << std::endl;

            Profile* userPtr = profilesMap.at(user);
            // Profile* toFollowPtr = profiles.at(toFollow);

            // Checks??
            userPtr->AddFollower(toFollow, false);
        }

        // Copy to profiles
        // for (auto it = profilesMap.begin(); it != profilesMap.end(); it++)
        // {
        //     profiles.push_back(it->second);
        // }

        return numProfiles;
    }
    else
    {
        std::cerr << "Could not open file" << std::endl;
        return 0;
    }
}

ProfileManager::ProfileManager() { int r = ReadProfilesFromFile(); }

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
    profilesFile.open(PROFILES_FILENAME, std::ios::out);
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