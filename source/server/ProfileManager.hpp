#include <string>
#include <vector>

class Profile
{
private:
    std::string handle;

public:
    Profile(std::string handle);

    std::string GetHandle();
};

class ProfileManager
{
private:
    std::vector<Profile> profiles;

    // Sincroniza os perfis em disco
    void Sync();

public:
    std::vector<Profile> GetProfiles();

    Profile NewProfile(std::string handle);
}; // namespace ProfileManager
