add_executable(Client main.cpp client.cpp)

# target_link_libraries(Client
#     #PUBLIC # Useful for libraries, see https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html for more details about transitive usage requirements.
#         #libraries/targets to link when linking this library
#         #this will automatically setup the needed flags and dependencies when linking against this target
#     PRIVATE # The following libraries are only linked for this target, and its flags/dependencies will not be used when linking against this target
#         general  # It is possible to link some libraries for debug or optimized builds only
#         #debug DEBUGLIBS
#         #optimized RELEASELIBS
# )

# include_directories(${CMAKE_SOURCE_DIR}/include)
target_set_warnings(Client DISABLE ALL)

target_link_libraries(Client PRIVATE Threads::Threads)

target_compile_features(Client PRIVATE cxx_std_14)
target_compile_options(Client PRIVATE -O0)

# target_set_warnings(Client ENABLE ALL AS_ERROR ALL DISABLE Annoying) # Helper that can set default warning flags for you
# target_enable_lto(Client optimized) #enable lto if available for non-debug configurations
# copy_dlls_for_debug(Client "" "") # Copy dependencies next to the executable (DLLs for example)

# set_directory_properties(PROPERTIES VS_STARTUP_PROJECT Client)