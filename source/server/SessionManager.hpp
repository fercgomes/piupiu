#include <map>
#include <mutex>
#include <thread>
#include "ProfileManager.hpp"

#define MAX_SESSIONS_PER_USER 2

struct Session
{
    int         sessionId;
    std::string userHandle;
    int         connectedSockets;
};

class SessionManager
{
public:
    SessionManager(ProfileManager* profileManager);
    ~SessionManager();

    // Retorna ponteiro para sessão, caso exista.
    // Retorna NULL caso o limite de sessões tenha
    // sido alcançado.
    Session* StartSession(std::string userHandle);
    int      EndSession(std::string userHandle);

    void print();

private:
    ProfileManager*                 profileManager;
    std::map<std::string, Session*> sessions;

    std::mutex sessionMutex;
};