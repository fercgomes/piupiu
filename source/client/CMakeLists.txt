add_executable(Client main.cpp client.cpp $<$<BOOL:${USE_GUI}>:gui/gui.cpp> ../messaging/Packet.cpp)

if(USE_GUI)
    target_compile_definitions(Client PUBLIC USE_GUI)
    find_package(glfw3 3.3 REQUIRED)
    find_package(OpenGL REQUIRED)
endif()

target_set_warnings(Client DISABLE ALL)


target_include_directories(Client PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                           $<INSTALL_INTERFACE:include>
                           "../messaging")               

target_link_libraries(Client PRIVATE Threads::Threads ${CMAKE_DL_LIBS} $<$<BOOL:${USE_GUI}>:imgui> $<$<BOOL:${USE_GUI}>:glfw> $<$<BOOL:${USE_GUI}>:OpenGL::GL>)

target_compile_features(Client PRIVATE cxx_std_14)
target_compile_options(Client PRIVATE -O0)