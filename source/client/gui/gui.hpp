#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "../client.hpp"

#define USERNAME_MAX_LEN 100

class GUI
{
public:
    int run();

    GUI(Client* client);

private:
    int init();
    int stop();
    int render();

    GLFWwindow* window = nullptr;
    Client*     client = nullptr;

    // Global GUI State
    ImVec4 clear_color;
    char   errorMessage[1024];
    bool   loggedIn = false;
    char   username[USERNAME_MAX_LEN];
};