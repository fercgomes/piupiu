#pragma once
#include <map>
#include <mutex>
#include <thread>
#include "ProfileManager.hpp"
#include "Session.hpp"

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
    Session* StartSession(Profile* profile, struct sockaddr_in sender);
    int      EndSession(Profile* profile, struct sockaddr_in sender);

    std::vector<struct sockaddr_in> GetUserAddresses(std::string handle);

    void print();
    int  GetUserNameByAddressAndIP(in_addr address, int port, std::string& out);

    ProfileManager* GetProfileManager() const;

private:
    ProfileManager*                 profileManager;
    std::map<std::string, Session*> sessions;

    std::mutex sessionMutex;
};