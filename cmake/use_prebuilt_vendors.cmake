# cmake/use_prebuilt_vendors.cmake
# Create IMPORTED targets from pre-built vendor libraries in BUGL_VENDOR_CACHE_DIR
# This avoids recompiling vendor libraries when the main build dir is deleted.

set(_VCD "${BUGL_VENDOR_CACHE_DIR}")
set(_SRC "${CMAKE_SOURCE_DIR}")

if(NOT EXISTS "${_VCD}")
    message(FATAL_ERROR
        "BUGL_USE_PREBUILT_VENDORS=ON but cache dir not found: ${_VCD}\n"
        "Build vendors first:\n"
        "  cmake -B build-vendor -G Ninja -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}\n"
        "  cmake --build build-vendor\n"
        "Then rebuild with: cmake -B build -DBUGL_USE_PREBUILT_VENDORS=ON")
endif()

message(STATUS "BuGL: using pre-built vendor libraries from ${_VCD}")

# Helper: find a .a in a directory and create an IMPORTED STATIC target
function(bugl_prebuilt_lib TARGET_NAME SEARCH_DIR)
    file(GLOB _libs "${SEARCH_DIR}/*.a")
    if(NOT _libs)
        message(FATAL_ERROR "bugl_prebuilt_lib(${TARGET_NAME}): no .a found in ${SEARCH_DIR}")
    endif()
    # If multiple .a, try to match target name
    set(_found "")
    foreach(_lib IN LISTS _libs)
        get_filename_component(_fname "${_lib}" NAME)
        string(TOLOWER "${_fname}" _fname_lower)
        string(TOLOWER "${TARGET_NAME}" _target_lower)
        if(_fname_lower MATCHES "${_target_lower}")
            set(_found "${_lib}")
            break()
        endif()
    endforeach()
    if(NOT _found)
        list(GET _libs 0 _found)
    endif()
    add_library(${TARGET_NAME} STATIC IMPORTED GLOBAL)
    set_target_properties(${TARGET_NAME} PROPERTIES IMPORTED_LOCATION "${_found}")
    # Forward extra args as include directories
    set(_inc_dirs ${ARGN})
    if(_inc_dirs)
        set_target_properties(${TARGET_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${_inc_dirs}")
    endif()
    message(STATUS "  prebuilt: ${TARGET_NAME} → ${_found}")
endfunction()

# ─── Core vendor libs (always needed) ───

bugl_prebuilt_lib(imgui "${_VCD}/imgui"
    "${_SRC}/vendor/imgui")
if(NOT BUGL_IMGUI_DEMO)
    set_target_properties(imgui PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "IMGUI_DISABLE_DEMO_WINDOWS")
endif()

bugl_prebuilt_lib(miniz "${_VCD}/miniz"
    "${_SRC}/vendor/miniz/include")

bugl_prebuilt_lib(poly2tri "${_VCD}/poly2tri"
    "${_SRC}/vendor/poly2tri"
    "${_SRC}/vendor/poly2tri/include")

# glfw (raylib dependency — internal, no public includes needed)
bugl_prebuilt_lib(glfw "${_VCD}/raylib/raylib/external/glfw/src")

# raylib
bugl_prebuilt_lib(raylib "${_VCD}/raylib/raylib"
    "${_SRC}/vendor/raylib/src")
# Transitive deps: glfw + system libs
find_package(Threads REQUIRED)
set_target_properties(raylib PROPERTIES
    INTERFACE_LINK_LIBRARIES "glfw;Threads::Threads;${CMAKE_DL_LIBS};m")

bugl_prebuilt_lib(bugl_audio "${_VCD}/audio"
    "${_SRC}/vendor/audio/include"
    "${_SRC}/vendor/audio/src")

# ─── Optional vendor libs ───

if(BUGL_WITH_BOX2D)
    bugl_prebuilt_lib(box2d "${_VCD}/box2d"
        "${_SRC}/vendor/box2d/include"
        "${_SRC}/main/src/box2d")
    set_target_properties(box2d PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "B2_USER_SETTINGS")
endif()

if(BUGL_WITH_JOLT)
    bugl_prebuilt_lib(Jolt "${_VCD}/Jolt"
        "${_SRC}/vendor/Jolt")
    # Propagate the same PUBLIC compile definitions Jolt uses by default
    set(_JOLT_DEFS "")
    # DEBUG_RENDERER_IN_DEBUG_AND_RELEASE defaults to ON → JPH_DEBUG_RENDERER for Debug/Release
    if(CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
        list(APPEND _JOLT_DEFS "JPH_DEBUG_RENDERER" "JPH_PROFILE_ENABLED")
    endif()
    # ENABLE_OBJECT_STREAM defaults to ON
    list(APPEND _JOLT_DEFS "JPH_OBJECT_STREAM")
    set_target_properties(Jolt PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "${_JOLT_DEFS}")
endif()

if(BUGL_WITH_ASSIMP)
    # Assimp also bundles zlib
    bugl_prebuilt_lib(zlibstatic "${_VCD}/assimp/contrib/zlib")

    bugl_prebuilt_lib(assimp "${_VCD}/assimp/lib"
        "${_SRC}/vendor/assimp/include"
        "${_VCD}/assimp/include")
    set_target_properties(assimp PROPERTIES
        INTERFACE_LINK_LIBRARIES "zlibstatic")
endif()

if(BUGL_WITH_MESHOPT)
    bugl_prebuilt_lib(meshoptimizer "${_VCD}/meshoptimizer"
        "${_SRC}/vendor/meshoptimizer/src")
endif()

if(BUGL_WITH_OPENSTEER)
    bugl_prebuilt_lib(libopensteer "${_VCD}/OpenSteer"
        "${_SRC}/vendor/OpenSteer/include")
    # Alias to match the target name used in main/CMakeLists.txt
    add_library(OpenSteer::Lib ALIAS libopensteer)
endif()

if(BUGL_WITH_RECAST)
    bugl_prebuilt_lib(Recast "${_VCD}/recastnavigation/Recast"
        "${_SRC}/vendor/recastnavigation/Recast/Include")

    bugl_prebuilt_lib(Detour "${_VCD}/recastnavigation/Detour"
        "${_SRC}/vendor/recastnavigation/Detour/Include")

    bugl_prebuilt_lib(DetourCrowd "${_VCD}/recastnavigation/DetourCrowd"
        "${_SRC}/vendor/recastnavigation/DetourCrowd/Include")
    set_target_properties(DetourCrowd PROPERTIES INTERFACE_LINK_LIBRARIES "Detour")

    bugl_prebuilt_lib(DetourTileCache "${_VCD}/recastnavigation/DetourTileCache"
        "${_SRC}/vendor/recastnavigation/DetourTileCache/Include")
    set_target_properties(DetourTileCache PROPERTIES INTERFACE_LINK_LIBRARIES "Detour")

    # Create namespaced aliases to match the target names in main/CMakeLists.txt
    add_library(RecastNavigation::Recast ALIAS Recast)
    add_library(RecastNavigation::Detour ALIAS Detour)
    add_library(RecastNavigation::DetourCrowd ALIAS DetourCrowd)
    add_library(RecastNavigation::DetourTileCache ALIAS DetourTileCache)
endif()

if(BUGL_WITH_CHIPMUNK)
    if(EXISTS "${_VCD}/Chipmunk2D")
        bugl_prebuilt_lib(chipmunk_static "${_VCD}/Chipmunk2D"
            "${_SRC}/vendor/Chipmunk2D/include")
    endif()
endif()
