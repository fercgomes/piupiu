#pragma once
#include <map>
#include <mutex>
#include <thread>
#include "ProfileManager.hpp"

#include <arpa/inet.h>

#define MAX_SESSIONS_PER_USER 2

struct Session
{
    int                                                   sessionId;
    std::string                                           userHandle;
    int                                                   connectedSockets;
    std::array<struct sockaddr_in, MAX_SESSIONS_PER_USER> connectedPeers;
};

struct Session2
{
    int                id;
    struct sockaddr_in address;
    Profile*           profile;
};

class SessionManager
{
public:
    SessionManager(ProfileManager* profileManager);
    ~SessionManager();

    // Retorna ponteiro para sessão, caso exista.
    // Retorna NULL caso o limite de sessões tenha
    // sido alcançado.
    Session* StartSession(std::string userHandle, struct sockaddr_in sender);
    int      EndSession(std::string userHandle, struct sockaddr_in sender);

    std::array<struct sockaddr_in, MAX_SESSIONS_PER_USER> GetUserAddresses(std::string handle);

    void print();

private:
    ProfileManager*                 profileManager;
    std::map<std::string, Session*> sessions;

    std::mutex sessionMutex;
};