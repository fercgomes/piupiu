#include "SessionManager.hpp"
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

Session* SessionManager::StartSession(std::string userHandle, struct sockaddr_in sender)
{
    // Esse seria o lugar certo de ativar o mutex?
    const std::lock_guard<std::mutex>         lock(sessionMutex);
    std::map<std::string, Session*>::iterator it;

    it = sessions.find(userHandle);
    if (it == sessions.end())
    {
        printf("No session exists for %s\n", userHandle.c_str());
        // Sessão não existe
        Session* newSession = new Session();
        memset(newSession, 0, sizeof(Session));

        newSession->sessionId         = 1;
        newSession->connectedSockets  = 1;
        newSession->userHandle        = userHandle;
        newSession->connectedPeers[0] = sender;

        sessions.insert(std::pair<std::string, Session*>(userHandle, newSession));
        return newSession;
    }
    else
    {
        // Sessão existe
        if (it->second->connectedSockets >= MAX_SESSIONS_PER_USER)
        {
            printf("Session for %s already exists (2)\n", it->first.c_str());
            return nullptr;
        }
        else
        {
            printf("Session for %s already exists (1)\n", it->first.c_str());
            it->second->connectedSockets++;
            it->second->connectedPeers[1] = sender;
            return (it->second);
        }
    }
}

int SessionManager::EndSession(std::string userHandle, struct sockaddr_in sender)
{
    const std::lock_guard<std::mutex> lock(sessionMutex);
    auto                              it = sessions.find(userHandle);

    if (it == sessions.end()) { return 0; }
    else
    {
        Session* session = it->second;
        if (session->connectedSockets == 1)
        {
            // TODO: erase sender address
            sessions.erase(it);
            delete session;
            return 1;
        }
        else
        {
            session->connectedSockets--;
            return 1;
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

std::string SessionManager::GetUserNameByAddressAndIP(in_addr address, int port)
{
    for (auto it = sessions.begin(); it != sessions.end(); ++it)
    {
        //TODO: need to check all elements on connectedPeers
        if (it->second->connectedPeers[0].sin_port == port && it->second->connectedPeers[0].sin_addr.s_addr == address.s_addr
        || it->second->connectedPeers[0].sin_port == port && it->second->connectedPeers[0].sin_addr.s_addr == address.s_addr)
            return it -> first;
    }
}

std::array<struct sockaddr_in, MAX_SESSIONS_PER_USER>
SessionManager::GetUserAddresses(std::string handle)
{
    auto it = sessions.find(handle);
    if (it == sessions.end()) { return std::array<struct sockaddr_in, MAX_SESSIONS_PER_USER>(); }
    else
    {
        return it->second->connectedPeers;
    }
}





