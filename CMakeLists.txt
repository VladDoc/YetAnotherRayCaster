cmake_minimum_required(VERSION 3.16)

project(RayCaster)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)

file (GLOB_RECURSE SOURCE_EXE "*.cpp" "*.h")

add_executable(raycaster ${SOURCE_EXE})
target_compile_features(raycaster PRIVATE cxx_std_17)

include_directories(include)

set(SDL_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/SDL)
set(SDL_LIBRARIES_DIR ${SDL_BUILD_DIR}/build)
set(SDL_LIBRARIES
    ${SDL_LIBRARIES_DIR}/SDL_active.o
    ${SDL_LIBRARIES_DIR}/SDL_alsa_audio.o
    ${SDL_LIBRARIES_DIR}/SDL_audiocvt.o
    ${SDL_LIBRARIES_DIR}/SDL_audiodev.o
    ${SDL_LIBRARIES_DIR}/SDL_audio.o
    ${SDL_LIBRARIES_DIR}/SDL_blit_0.o
    ${SDL_LIBRARIES_DIR}/SDL_blit_1.o
    ${SDL_LIBRARIES_DIR}/SDL_blit_A.o
    ${SDL_LIBRARIES_DIR}/SDL_blit_N.o
    ${SDL_LIBRARIES_DIR}/SDL_blit.o
    ${SDL_LIBRARIES_DIR}/SDL_bmp.o
    ${SDL_LIBRARIES_DIR}/SDL_cdrom.o
    ${SDL_LIBRARIES_DIR}/SDL_cpuinfo.o
    ${SDL_LIBRARIES_DIR}/SDL_cursor.o
    ${SDL_LIBRARIES_DIR}/SDL_dgaevents.o
    ${SDL_LIBRARIES_DIR}/SDL_dgamouse.o
    ${SDL_LIBRARIES_DIR}/SDL_dgavideo.o
    ${SDL_LIBRARIES_DIR}/SDL_diskaudio.o
    ${SDL_LIBRARIES_DIR}/SDL_dmaaudio.o
    ${SDL_LIBRARIES_DIR}/SDL_dspaudio.o
    ${SDL_LIBRARIES_DIR}/SDL_dummyaudio.o
    ${SDL_LIBRARIES_DIR}/SDL_dummy_main.o
    ${SDL_LIBRARIES_DIR}/SDL_error.o
    ${SDL_LIBRARIES_DIR}/SDL_events.o
    ${SDL_LIBRARIES_DIR}/SDL_expose.o
    ${SDL_LIBRARIES_DIR}/SDL_fatal.o
    ${SDL_LIBRARIES_DIR}/SDL_fb3dfx.o
    ${SDL_LIBRARIES_DIR}/SDL_fbelo.o
    ${SDL_LIBRARIES_DIR}/SDL_fbevents.o
    ${SDL_LIBRARIES_DIR}/SDL_fbmatrox.o
    ${SDL_LIBRARIES_DIR}/SDL_fbmouse.o
    ${SDL_LIBRARIES_DIR}/SDL_fbriva.o
    ${SDL_LIBRARIES_DIR}/SDL_fbvideo.o
    ${SDL_LIBRARIES_DIR}/SDL_gamma.o
    ${SDL_LIBRARIES_DIR}/SDL_getenv.o
    ${SDL_LIBRARIES_DIR}/SDL_iconv.o
    ${SDL_LIBRARIES_DIR}/SDL_joystick.o
    ${SDL_LIBRARIES_DIR}/SDL_keyboard.o
    ${SDL_LIBRARIES_DIR}/SDL_malloc.o
    ${SDL_LIBRARIES_DIR}/SDL_mixer_m68k.o
    ${SDL_LIBRARIES_DIR}/SDL_mixer_MMX.o
    ${SDL_LIBRARIES_DIR}/SDL_mixer_MMX_VC.o
    ${SDL_LIBRARIES_DIR}/SDL_mixer.o
    ${SDL_LIBRARIES_DIR}/SDL_mouse.o
    ${SDL_LIBRARIES_DIR}/SDL_nullevents.o
    ${SDL_LIBRARIES_DIR}/SDL_nullmouse.o
    ${SDL_LIBRARIES_DIR}/SDL_nullvideo.o
    ${SDL_LIBRARIES_DIR}/SDL.o
    ${SDL_LIBRARIES_DIR}/SDL_pixels.o
    ${SDL_LIBRARIES_DIR}/SDL_pulseaudio.o
    ${SDL_LIBRARIES_DIR}/SDL_qsort.o
    ${SDL_LIBRARIES_DIR}/SDL_quit.o
    ${SDL_LIBRARIES_DIR}/SDL_resize.o
    ${SDL_LIBRARIES_DIR}/SDL_RLEaccel.o
    ${SDL_LIBRARIES_DIR}/SDL_rwops.o
    ${SDL_LIBRARIES_DIR}/SDL_stdlib.o
    ${SDL_LIBRARIES_DIR}/SDL_stretch.o
    ${SDL_LIBRARIES_DIR}/SDL_string.o
    ${SDL_LIBRARIES_DIR}/SDL_surface.o
    ${SDL_LIBRARIES_DIR}/SDL_syscdrom.o
    ${SDL_LIBRARIES_DIR}/SDL_syscond.o
    ${SDL_LIBRARIES_DIR}/SDL_sysjoystick.o
    ${SDL_LIBRARIES_DIR}/SDL_sysloadso.o
    ${SDL_LIBRARIES_DIR}/SDL_sysmutex.o
    ${SDL_LIBRARIES_DIR}/SDL_syssem.o
    ${SDL_LIBRARIES_DIR}/SDL_systhread.o
    ${SDL_LIBRARIES_DIR}/SDL_systimer.o
    ${SDL_LIBRARIES_DIR}/SDL_thread.o
    ${SDL_LIBRARIES_DIR}/SDL_timer.o
    ${SDL_LIBRARIES_DIR}/SDL_video.o
    ${SDL_LIBRARIES_DIR}/SDL_wave.o
    ${SDL_LIBRARIES_DIR}/SDL_x11dga.o
    ${SDL_LIBRARIES_DIR}/SDL_x11dyn.o
    ${SDL_LIBRARIES_DIR}/SDL_x11events.o
    ${SDL_LIBRARIES_DIR}/SDL_x11gamma.o
    ${SDL_LIBRARIES_DIR}/SDL_x11gl.o
    ${SDL_LIBRARIES_DIR}/SDL_x11image.o
    ${SDL_LIBRARIES_DIR}/SDL_x11modes.o
    ${SDL_LIBRARIES_DIR}/SDL_x11mouse.o
    ${SDL_LIBRARIES_DIR}/SDL_x11video.o
    ${SDL_LIBRARIES_DIR}/SDL_x11wm.o
    ${SDL_LIBRARIES_DIR}/SDL_x11yuv.o
    ${SDL_LIBRARIES_DIR}/SDL_yuv_mmx.o
    ${SDL_LIBRARIES_DIR}/SDL_yuv.o
    ${SDL_LIBRARIES_DIR}/SDL_yuv_sw.o
    ${SDL_LIBRARIES_DIR}/XF86DGA2.o
    ${SDL_LIBRARIES_DIR}/XF86DGA.o
    ${SDL_LIBRARIES_DIR}/XF86VMode.o
    ${SDL_LIBRARIES_DIR}/Xinerama.o
    ${SDL_LIBRARIES_DIR}/xme.o
    ${SDL_LIBRARIES_DIR}/Xv.o
    )
add_library(sdl STATIC ${SDL_LIBRARIES})
set_target_properties(sdl PROPERTIES LINKER_LANGUAGE CXX)

if(NOT EXISTS ${SDL_LIBRARIES_DIR})
    file(COPY lib/SDL DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    add_custom_command(
        OUTPUT ${SDL_LIBRARIES}
        WORKING_DIRECTORY ${SDL_BUILD_DIR}
        COMMAND ./autogen.sh && ./configure && make
    )
    add_custom_target(make_sdl DEPENDS ${SDL_LIBRARIES})
    add_dependencies(sdl make_sdl)
endif()

file(COPY lib/lua DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(LUA_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/lua)
set(LUA_LIBRARIES
    ${LUA_BUILD_DIR}/lapi.o
    ${LUA_BUILD_DIR}/lauxlib.o
    ${LUA_BUILD_DIR}/lbaselib.o
    ${LUA_BUILD_DIR}/lcode.o
    ${LUA_BUILD_DIR}/lcorolib.o
    ${LUA_BUILD_DIR}/lctype.o
    ${LUA_BUILD_DIR}/ldblib.o
    ${LUA_BUILD_DIR}/ldebug.o
    ${LUA_BUILD_DIR}/ldo.o
    ${LUA_BUILD_DIR}/ldump.o
    ${LUA_BUILD_DIR}/lfunc.o
    ${LUA_BUILD_DIR}/lgc.o
    ${LUA_BUILD_DIR}/linit.o
    ${LUA_BUILD_DIR}/liolib.o
    ${LUA_BUILD_DIR}/llex.o
    ${LUA_BUILD_DIR}/lmathlib.o
    ${LUA_BUILD_DIR}/lmem.o
    ${LUA_BUILD_DIR}/loadlib.o
    ${LUA_BUILD_DIR}/lobject.o
    ${LUA_BUILD_DIR}/lopcodes.o
    ${LUA_BUILD_DIR}/loslib.o
    ${LUA_BUILD_DIR}/lparser.o
    ${LUA_BUILD_DIR}/lstate.o
    ${LUA_BUILD_DIR}/lstring.o
    ${LUA_BUILD_DIR}/lstrlib.o
    ${LUA_BUILD_DIR}/ltable.o
    ${LUA_BUILD_DIR}/ltablib.o
    ${LUA_BUILD_DIR}/ltests.o
    ${LUA_BUILD_DIR}/ltm.o
    ${LUA_BUILD_DIR}/lua.o
    ${LUA_BUILD_DIR}/lundump.o
    ${LUA_BUILD_DIR}/lutf8lib.o
    ${LUA_BUILD_DIR}/lvm.o
    ${LUA_BUILD_DIR}/lzio.o
    )
set(LUA_DEPENDENCIES
    dl
    ts)
add_library(lua STATIC ${LUA_LIBRARIES})
set_target_properties(lua PROPERTIES LINKER_LANGUAGE CXX)

add_custom_command(
    OUTPUT ${LUA_LIBRARIES}
    WORKING_DIRECTORY ${LUA_BUILD_DIR}
    COMMAND make
)
add_custom_target(make_lua DEPENDS ${LUA_LIBRARIES})
add_dependencies(lua make_lua)

target_link_libraries(raycaster
    lua
    ${LUA_DEPENDENCIES}
    pthread
    sdl
    )

file(COPY resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR})