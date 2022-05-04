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

    // Retorna ponteiro para sess√£o, caso exista.
    // Retorna NULL caso o limite de sess√µes tenha
    // sido alcan√ßado.
    Session* StartSession(Profile* profile, SocketAddress sender);
    int      EndSession(Profile* profile, SocketAddress sender);

    std::vector<SocketAddress> GetUserAddresses(std::string handle);

    void print();
    int  GetUserNameByAddressAndIP(SocketAddress address, std::string& out);

    ProfileManager* GetProfileManager() const;

    // TODO - FunÁ„o para retornar a lista de clients
    std::vector<Session*> GetSessions();

private:
    ProfileManager*                 profileManager;
    std::map<std::string, Session*> sessions;

    std::mutex sessionMutex;
};