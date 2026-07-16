cmake_minimum_required(VERSION 3.16)
project(SpaceCadetPinball)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

# --- Source files ---
file(GLOB_RECURSE SOURCE_FILES SpaceCadetPinball/*.cpp SpaceCadetPinball/*.h)

if(WIN32)
    set_source_files_properties(SpaceCadetPinball/SpaceCadetPinball.rc PROPERTIES LANGUAGE RC)
    list(APPEND SOURCE_FILES SpaceCadetPinball/SpaceCadetPinball.rc)
endif()

add_executable(SpaceCadetPinball ${SOURCE_FILES})

# --- Precompiled headers ---
set_source_files_properties(
    SpaceCadetPinball/imgui.cpp
    SpaceCadetPinball/imgui_sdl.cpp
    PROPERTIES SKIP_PRECOMPILE_HEADERS 1
)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    target_precompile_headers(SpaceCadetPinball PUBLIC SpaceCadetPinball/pch.h)
endif()

# --- Dependências por plataforma ---
if(WIN32)
    # SDL2 para cross-compile Windows
    set(SDL2_PATH "/home/diego/SDL2-win64")
    set(SDL2_INCLUDE_DIR "${SDL2_PATH}/include/SDL2")
    set(SDL2_LIBRARY "${SDL2_PATH}/lib/libSDL2.a")

    set(SDL2_MIXER_PATH "/home/diego/SDL2_mixer-win64")
    set(SDL2_MIXER_INCLUDE_DIR "${SDL2_MIXER_PATH}/include/SDL2")
    set(SDL2_MIXER_LIBRARY "${SDL2_MIXER_PATH}/lib/libSDL2_mixer.a")

    include_directories(${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR})

    target_link_libraries(
        SpaceCadetPinball
        ${SDL2_LIBRARY}
        ${SDL2_MIXER_LIBRARY}
        setupapi
        imm32
        winmm
        version
        cfgmgr32
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -static-libgcc -static-libstdc++ -static")

elseif(PSP)
    # SDL2 no PSP (via pkg-config)
    include(FindPkgConfig)
    pkg_search_module(SDL2 REQUIRED sdl2)
    pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)

    target_include_directories(SpaceCadetPinball PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
    )

    target_link_libraries(SpaceCadetPinball
        ${SDL2_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        pspdebug
        pspdisplay
        pspge
        pspctrl
        psputility
        m
    )

    create_pbp_file(
        TARGET ${PROJECT_NAME}
        ICON_PATH NULL
        BACKGROUND_PATH NULL
        PREVIEW_PATH NULL
        TITLE ${PROJECT_NAME}
        VERSION 01.00
    )
endif()