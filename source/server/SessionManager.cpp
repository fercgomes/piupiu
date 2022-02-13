#include "SessionManager.hpp"
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

Session* SessionManager::StartSession(std::string userHandle)
{
    // Esse seria o lugar certo de ativar o mutex?
    const std::lock_guard<std::mutex>         lock(sessionMutex);
    std::map<std::string, Session*>::iterator it;

    it = sessions.find(userHandle);
    if (it == sessions.end())
    {
        // Sessão não existe
        Session* newSession          = new Session();
        newSession->sessionId        = 1;
        newSession->connectedSockets = 1;
        newSession->userHandle       = userHandle;

        sessions.insert(std::pair<std::string, Session*>(userHandle, newSession));
        return newSession;
    }
    else
    {
        // Sessão existe
        if (it->second->connectedSockets > MAX_SESSIONS_PER_USER) { return nullptr; }
        else
        {
            it->second->connectedSockets++;
            return (it->second);
        }
    }
}

int SessionManager::EndSession(std::string userHandle)
{
    const std::lock_guard<std::mutex> lock(sessionMutex);
    auto                              it = sessions.find(userHandle);

    if (it == sessions.end()) { return 0; }
    else
    {
        Session* session = it->second;
        if (session->connectedSockets == 1)
        {
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