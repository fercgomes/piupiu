#include <Notification.hpp>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

class Profile
{
private:
    std::string              handle;
    ProfileManager*          manager;
    std::vector<Profile*>    following;
    std::queue<Notification> pendingNotifications;
    std::mutex               queueMutex;

public:
    Profile(std::string handle, ProfileManager* manager);

    std::string           GetHandle();
    std::vector<Profile*> GetFollowers();
    void                  Follow(std::string userHandle);
    int                   PushNotification(Notification notification);
};

class ProfileManager
{
private:
    std::vector<Profile*> profiles;

    // Sincroniza os perfis em disco
    void Sync();

public:
    std::vector<Profile*> GetProfiles();
    Profile*              GetProfileByName(std::string userHandle);

    Profile* NewProfile(std::string handle);
}; // namespace ProfileManager
