#pragma once
#include <map>
#include <mutex>
#include <thread>
#include "ProfileManager.hpp"
#include "Session.hpp"
#include "Socket.hpp"

#include <arpa/inet.h>

#define MAX_SESSIONS_PER_USER 2

class SessionManager
{
public:
    SessionManager(ProfileManager* profileManager);
    ~SessionManager();

    // Retorna ponteiro para sessão, caso exista.
    // Retorna NULL caso o limite de sessões tenha
    // sido alcançado.
    Session* StartSession(Profile* profile, SocketAddress sender);
    int      EndSession(Profile* profile, SocketAddress sender);

    std::vector<SocketAddress> GetUserAddresses(std::string handle);

    void print();
    int  GetUserNameByAddressAndIP(SocketAddress address, std::string& out);

    ProfileManager* GetProfileManager() const;

    Session* GetSessions() const;

private:
    ProfileManager*                 profileManager;
    std::map<std::string, Session*> sessions;

    std::mutex sessionMutex;
};