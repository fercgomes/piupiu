#include "SessionManager.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

SessionManager::SessionManager(ProfileManager* profileManager) : profileManager(profileManager) {}

SessionManager::~SessionManager()
{
    // Desaloca os pointeiros de sessão
    for (auto it = sessions.begin(); it != sessions.end(); ++it)
    {
        delete it->second;
    }
}

std::vector<Session*> SessionManager::GetSessions()
{
   std::vector<Session*> clientList; 
   for (std::map<std::string, Session*>::const_iterator it=sessions.begin(); it!=sessions.end(); ++it)
   {
        clientList.push_back( it->second );
   }

   return clientList;
}


Session* SessionManager::StartSession(Profile* profile, SocketAddress sender)
{
    // Esse seria o lugar certo de ativar o mutex?
    const std::lock_guard<std::mutex>         lock(sessionMutex);
    std::map<std::string, Session*>::iterator it;
    std::string                               userHandle = profile->GetHandle();

    it = sessions.find(userHandle);
    if (it == sessions.end())
    {
        printf("No session exists for %s\n", userHandle.c_str());
        // Sessão não existe
        Session* newSession = new Session();

        newSession->sessionId  = 1;
        newSession->userHandle = userHandle;
        newSession->sockets.push_back(sender);

        sessions.insert(std::pair<std::string, Session*>(userHandle, newSession));
        return newSession;
    }
    else
    {
        // Sessão existe
        if (it->second->sockets.size() >= MAX_SESSIONS_PER_USER)
        {
            printf("Session for %s already exists (2)\n", it->first.c_str());
            return nullptr;
        }
        else
        {
            printf("Session for %s already exists (1)\n", it->first.c_str());
            it->second->sockets.push_back(sender);
            return (it->second);
        }
    }
}

int SessionManager::EndSession(Profile* profile, SocketAddress sender)
{
    const std::lock_guard<std::mutex> lock(sessionMutex);

    std::string userHandle = profile->GetHandle();
    auto        it         = sessions.find(userHandle);

    if (it == sessions.end()) { return 0; }
    else
    {
        Session* session = it->second;

        auto socketIt = std::find_if(
            session->sockets.begin(), session->sockets.end(), [sender](SocketAddress a) {
                return a.address == sender.address && a.port == sender.port;
            });

        if (socketIt != session->sockets.end())
        {
            session->sockets.erase(socketIt);
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

void SessionManager::print()
{
    for (auto it = sessions.begin(); it != sessions.end(); ++it)
    {
        printf("%s: \n", it->second->userHandle);
    }
}

int SessionManager::GetUserNameByAddressAndIP(SocketAddress address, std::string& out)
{
    const std::lock_guard<std::mutex> lock(sessionMutex);

    bool found = false;
    for (auto it = sessions.begin(); it != sessions.end(); ++it)
    {
        for (auto socket : it->second->sockets)
        {
            if (socket.address == address.address && socket.port == address.port)
            {
                found     = true;
                auto user = it->second->userHandle;
                out.swap(user);
                return 1;
            }
        }
    }

    return 0;
}

std::vector<SocketAddress> SessionManager::GetUserAddresses(std::string handle)
{
    const std::lock_guard<std::mutex> lock(sessionMutex);

    auto it = sessions.find(handle);
    if (it == sessions.end()) { return std::vector<SocketAddress>(); }
    else
    {
        return it->second->sockets;
    }
}

ProfileManager* SessionManager::GetProfileManager() const { return this->profileManager; }