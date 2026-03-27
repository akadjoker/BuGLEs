cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0072 NEW)

project(bugl VERSION 2.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUGL_PORTABLE_RELEASE "Build release binaries without CPU-specific native flags" ON)
option(BUGL_STRIP_RELEASE "Strip symbols from release binaries when supported" ON)
option(BUGL_IMGUI_DEMO "Build Dear ImGui demo window support" OFF)
option(BUGL_ENABLE_COMPILER_CACHE "Use compiler cache launcher (sccache/ccache) when available" ON)

# Optional modules (all ON by default — disable with -DBUGL_WITH_XXX=OFF)
option(BUGL_WITH_BOX2D      "Build Box2D 2D physics module"          ON)
option(BUGL_WITH_JOLT       "Build Jolt 3D physics module"           ON)
option(BUGL_WITH_CHIPMUNK   "Build Chipmunk2D physics module"        OFF)
option(BUGL_WITH_ASSIMP     "Build Assimp 3D model loading module"   ON)
option(BUGL_WITH_MESHOPT    "Build MeshOptimizer module"             ON)
option(BUGL_WITH_MICROPATHER "Build MicroPather pathfinding module"  ON)
option(BUGL_WITH_OPENSTEER  "Build OpenSteer AI steering module"     ON)
option(BUGL_WITH_RECAST     "Build Recast navigation mesh module"    ON)

# Persistent vendor build cache: vendor .a/.o files survive "rm -rf build/"
option(BUGL_VENDOR_CACHE "Build vendor libraries in a persistent directory that survives build cleanup" ON)
option(BUGL_USE_PREBUILT_VENDORS "Skip vendor compilation — use pre-built .a from cache (much faster after rm -rf build/)" OFF)
set(BUGL_VENDOR_CACHE_DIR "${CMAKE_SOURCE_DIR}/build-vendor-cache/${CMAKE_BUILD_TYPE}"
    CACHE PATH "Persistent build directory for vendor libraries")

# Helper: add_subdirectory with optional persistent cache binary dir
macro(bugl_vendor_subdir relative_path)
    if(BUGL_VENDOR_CACHE)
        get_filename_component(_bugl_vname "${relative_path}" NAME)
        add_subdirectory("${relative_path}" "${BUGL_VENDOR_CACHE_DIR}/${_bugl_vname}")
    else()
        add_subdirectory("${relative_path}")
    endif()
endmacro()

if(BUGL_ENABLE_COMPILER_CACHE)
    if(NOT CMAKE_C_COMPILER_LAUNCHER AND NOT CMAKE_CXX_COMPILER_LAUNCHER)
        find_program(BUGL_COMPILER_CACHE_PROGRAM NAMES sccache ccache)
        if(BUGL_COMPILER_CACHE_PROGRAM)
            message(STATUS "BuGL: enabling compiler cache launcher: ${BUGL_COMPILER_CACHE_PROGRAM}")
            set(CMAKE_C_COMPILER_LAUNCHER "${BUGL_COMPILER_CACHE_PROGRAM}" CACHE STRING "C compiler launcher" FORCE)
            set(CMAKE_CXX_COMPILER_LAUNCHER "${BUGL_COMPILER_CACHE_PROGRAM}" CACHE STRING "CXX compiler launcher" FORCE)
        else()
            message(STATUS "BuGL: compiler cache launcher not found (sccache/ccache)")
        endif()
    endif()
endif()

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
    add_custom_target(
        tidy
        COMMAND ${CLANG_TIDY_EXE}
        -p=${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/libbu/src/*.cpp
        ${CMAKE_SOURCE_DIR}/main/src/*.cpp
        COMMENT "Running clang-tidy"
    )
endif()

# --- Vendor libraries ---
if(BUGL_USE_PREBUILT_VENDORS)
    # Fast path: skip all vendor compilation, use cached .a files directly
    include(${CMAKE_SOURCE_DIR}/cmake/use_prebuilt_vendors.cmake)
else()
    # Normal path: build vendors from source (cached to persistent dir if BUGL_VENDOR_CACHE=ON)
    if(BUGL_VENDOR_CACHE)
        message(STATUS "BuGL: vendor cache ON → ${BUGL_VENDOR_CACHE_DIR}")
    endif()

    bugl_vendor_subdir(vendor/imgui)
    if(TARGET imgui)
        set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    bugl_vendor_subdir(vendor/miniz)
    if(TARGET miniz)
        set_target_properties(miniz PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    bugl_vendor_subdir(vendor/poly2tri)

    # Raylib — static, no audio (we have our own), no examples
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "Disable raylib examples" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "Disable raylib games" FORCE)
    set(USE_AUDIO OFF CACHE BOOL "Disable raylib audio (BuGL uses its own)" FORCE)
    set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
    bugl_vendor_subdir(vendor/raylib)
    if(TARGET raylib)
        set_target_properties(raylib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    if(BUGL_WITH_BOX2D)
        bugl_vendor_subdir(vendor/box2d)
        if(TARGET box2d)
            target_compile_definitions(box2d PUBLIC B2_USER_SETTINGS)
            target_include_directories(box2d PUBLIC ${CMAKE_SOURCE_DIR}/main/src/box2d)
            set_target_properties(box2d PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()
    endif()

    bugl_vendor_subdir(vendor/audio)

    if(BUGL_WITH_OPENSTEER)
        set(OPENSTEER_BUILD_DEMO OFF CACHE BOOL "Disable upstream OpenSteer demo in BuGL build" FORCE)
        bugl_vendor_subdir(vendor/OpenSteer)
    endif()

    if(BUGL_WITH_JOLT)
        bugl_vendor_subdir(vendor/Jolt)
    endif()

    if(BUGL_WITH_CHIPMUNK)
        set(BUILD_DEMOS OFF CACHE BOOL "Disable Chipmunk2D demos in BuGL build" FORCE)
        set(BUILD_SHARED OFF CACHE BOOL "Disable Chipmunk2D shared library in BuGL build" FORCE)
        set(BUILD_STATIC ON CACHE BOOL "Enable Chipmunk2D static library in BuGL build" FORCE)
        set(INSTALL_STATIC OFF CACHE BOOL "Disable Chipmunk2D install target in BuGL build" FORCE)
        bugl_vendor_subdir(vendor/Chipmunk2D)
        if(TARGET chipmunk_static)
            target_include_directories(chipmunk_static PUBLIC ${CMAKE_SOURCE_DIR}/vendor/Chipmunk2D/include)
            set_target_properties(chipmunk_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()
    endif()

    if(BUGL_WITH_ODE)
        if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/ODE/CMakeLists.txt")
            set(ODE_WITH_DEMOS OFF CACHE BOOL "Disable ODE demos in BuGL build" FORCE)
            set(ODE_WITH_TESTS OFF CACHE BOOL "Disable ODE tests in BuGL build" FORCE)
            bugl_vendor_subdir(vendor/ODE)
        else()
            message(WARNING "BUGL_WITH_ODE enabled but vendor/ODE not found – skipping")
        endif()
    endif()

    if(BUGL_WITH_ASSIMP)
        set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
        set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
        set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
        bugl_vendor_subdir(vendor/assimp)
    endif()

    if(BUGL_WITH_MESHOPT)
        bugl_vendor_subdir(vendor/meshoptimizer)
    endif()

    if(BUGL_WITH_RECAST)
        set(RECASTNAVIGATION_DEMO OFF CACHE BOOL "Disable Recast demo" FORCE)
        set(RECASTNAVIGATION_EXAMPLES OFF CACHE BOOL "Disable Recast examples" FORCE)
        set(RECASTNAVIGATION_TESTS OFF CACHE BOOL "Disable Recast tests" FORCE)
        bugl_vendor_subdir(vendor/recastnavigation)
    endif()
endif()

# micropather is tiny (single file) — always build from source
if(BUGL_WITH_MICROPATHER)
    add_library(micropather STATIC vendor/MicroPather/micropather.cpp)
    target_include_directories(micropather PUBLIC vendor/MicroPather)
    set_target_properties(micropather PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

add_subdirectory(libbu)
add_subdirectory(main)

# Language test suite (lightweight: only needs libbu)
enable_testing()
add_subdirectory(tests)
 

 
