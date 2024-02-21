cmake_minimum_required(VERSION 3.22)

project(RayCaster)

file (GLOB SOURCE_EXE "*.cpp" "*.h")

add_executable(raycaster ${SOURCE_EXE})
target_compile_features(raycaster PRIVATE cxx_std_17)

include_directories(include SDL_SOURCE_DIR)

include(FetchContent)

FetchContent_Declare(SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL-1.2.git
    GIT_TAG release-1.2.15
)

find_package(SDL REQUIRED)

add_library(SDL STATIC ${SDL_LIBRARIES})
set_target_properties(SDL PROPERTIES LINKER_LANGUAGE CXX)

#if(NOT EXISTS ${SDL_LIBRARIES_DIR})
#    file(COPY lib/SDL DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#    add_custom_command(
#        OUTPUT ${SDL_LIBRARIES}
#        WORKING_DIRECTORY ${SDL_BUILD_DIR}
#        COMMAND ./autogen.sh && ./configure && make
#    )
#    add_custom_target(make_sdl DEPENDS ${SDL_LIBRARIES})
#    add_dependencies(sdl make_sdl)
#endif()

set(BUILD_SHARED_LIBS FALSE)

add_subdirectory(external/lua)

set_target_properties(SDL PROPERTIES LINKER_LANGUAGE CXX)


target_link_libraries(raycaster
    lua::lib
    ${LUA_DEPENDENCIES}
    pthread
    SDL::SDL
    )