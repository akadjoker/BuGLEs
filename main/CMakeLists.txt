project(main)
cmake_policy(SET CMP0072 NEW)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -D_CRT_SECURE_NO_WARNINGS")
    if (MSVC)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            add_compile_options(/RTC1 /Od /Zi)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
        endif()     
    endif()

endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB RAY_SOURCES CONFIGURE_DEPENDS "src/ray/*.cpp")

# Collect optional module sources
set(MODULE_SOURCES "")
set(MODULE_LIBS "")
set(MODULE_DEFS "")

if(BUGL_WITH_BOX2D)
    file(GLOB BOX2D_SRCS CONFIGURE_DEPENDS "src/box2d/*.cpp")
    list(APPEND MODULE_SOURCES ${BOX2D_SRCS})
    list(APPEND MODULE_LIBS box2d)
    list(APPEND MODULE_DEFS BUGL_WITH_BOX2D)
endif()

if(BUGL_WITH_JOLT)
    file(GLOB JOLT_SRCS CONFIGURE_DEPENDS "src/jolt/*.cpp")
    list(APPEND MODULE_SOURCES ${JOLT_SRCS})
    list(APPEND MODULE_LIBS Jolt)
    list(APPEND MODULE_DEFS BUGL_WITH_JOLT)
endif()

if(BUGL_WITH_ASSIMP)
    file(GLOB ASSIMP_SRCS CONFIGURE_DEPENDS "src/assimp/*.cpp")
    list(APPEND MODULE_SOURCES ${ASSIMP_SRCS})
    list(APPEND MODULE_LIBS assimp)
    list(APPEND MODULE_DEFS BUGL_WITH_ASSIMP)
endif()

if(BUGL_WITH_MESHOPT)
    file(GLOB MESHOPT_SRCS CONFIGURE_DEPENDS "src/meshopt/*.cpp")
    list(APPEND MODULE_SOURCES ${MESHOPT_SRCS})
    list(APPEND MODULE_LIBS meshoptimizer)
    list(APPEND MODULE_DEFS BUGL_WITH_MESHOPT)
endif()

if(BUGL_WITH_MICROPATHER)
    file(GLOB MICROPATHER_SRCS CONFIGURE_DEPENDS "src/micropather/*.cpp")
    list(APPEND MODULE_SOURCES ${MICROPATHER_SRCS})
    list(APPEND MODULE_LIBS micropather)
    list(APPEND MODULE_DEFS BUGL_WITH_MICROPATHER)
endif()

if(BUGL_WITH_OPENSTEER)
    file(GLOB OPENSTEER_SRCS CONFIGURE_DEPENDS "src/opensteer/*.cpp")
    list(APPEND MODULE_SOURCES ${OPENSTEER_SRCS})
    list(APPEND MODULE_LIBS OpenSteer::Lib)
    list(APPEND MODULE_DEFS BUGL_WITH_OPENSTEER)
endif()

if(BUGL_WITH_RECAST)
    file(GLOB RECAST_SRCS CONFIGURE_DEPENDS "src/recast/*.cpp")
    list(APPEND MODULE_SOURCES ${RECAST_SRCS})
    list(APPEND MODULE_LIBS RecastNavigation::Recast RecastNavigation::Detour RecastNavigation::DetourCrowd RecastNavigation::DetourTileCache)
    list(APPEND MODULE_DEFS BUGL_WITH_RECAST)
endif()

if(BUGL_WITH_ODE)
    list(APPEND MODULE_LIBS ODE)
    list(APPEND MODULE_DEFS BUGL_WITH_ODE)
endif()

if(BUGL_WITH_CHIPMUNK)
    list(APPEND MODULE_LIBS chipmunk_static)
    list(APPEND MODULE_DEFS BUGL_WITH_CHIPMUNK)
endif()

# Filter out plugin.cpp from module sources (no longer needed)
list(FILTER MODULE_SOURCES EXCLUDE REGEX "plugin\\.cpp$")

add_executable(main ${SOURCES} ${RAY_SOURCES} ${MODULE_SOURCES})

set_target_properties(main PROPERTIES OUTPUT_NAME "bugl")
target_include_directories(main PUBLIC include src 
    ${CMAKE_SOURCE_DIR}/vendor/imgui
    ${CMAKE_SOURCE_DIR}/vendor/raylib/src
    ${CMAKE_SOURCE_DIR}/main/src
)
target_compile_definitions(main PRIVATE ${MODULE_DEFS})

if (WIN32)
    target_compile_definitions(main PRIVATE WINGDIAPI=)
    if (NOT MSVC)
        target_compile_options(main PRIVATE -U_DLL)
    endif()
endif()
if(CMAKE_BUILD_TYPE MATCHES Debug)

if (UNIX)
target_compile_options(main PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG -DVERBOSE)
target_link_options(main PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG) 
endif()
 
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(main PRIVATE
        -O3
        -DNDEBUG
        -ftree-vectorize
        -finline-functions
        -funroll-loops
    )

    if (NOT BUGL_PORTABLE_RELEASE)
        target_compile_options(main PRIVATE -march=native -mtune=native)
    endif()

    if (BUGL_STRIP_RELEASE AND NOT MSVC)
        target_link_options(main PRIVATE -s)
    endif()
endif()

set(BUGL_CORE_LIBS miniz poly2tri bugl_audio libbu imgui raylib)

target_link_libraries(main PRIVATE ${BUGL_CORE_LIBS} ${MODULE_LIBS})

# Assimp bundles its own poly2tri — allow duplicate symbols
if(BUGL_WITH_ASSIMP AND NOT MSVC)
    target_link_options(main PRIVATE "LINKER:--allow-multiple-definition")
endif()

if (WIN32)
    target_link_libraries(main PRIVATE opengl32)
else()
    target_link_libraries(main PRIVATE GL)
endif()

if (WIN32)
    if (MSVC)
        target_link_libraries(main PRIVATE Winmm.lib)
    else()
        target_link_libraries(main PRIVATE winmm)
    endif()
endif()

if (MINGW)
    get_filename_component(BUGL_MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(BUGL_MINGW_RUNTIME_DLLS
        libstdc++-6.dll
        libwinpthread-1.dll
        libgcc_s_seh-1.dll
        libgcc_s_dw2-1.dll
        libgcc_s_sjlj-1.dll
    )

    foreach(BUGL_RUNTIME_DLL IN LISTS BUGL_MINGW_RUNTIME_DLLS)
        if(EXISTS "${BUGL_MINGW_BIN_DIR}/${BUGL_RUNTIME_DLL}")
            add_custom_command(TARGET main POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${BUGL_MINGW_BIN_DIR}/${BUGL_RUNTIME_DLL}"
                    "$<TARGET_FILE_DIR:main>/${BUGL_RUNTIME_DLL}"
                COMMENT "Copying ${BUGL_RUNTIME_DLL} to output directory"
            )
        endif()
    endforeach()
endif()


if (UNIX)
    target_link_libraries(main PRIVATE m)
endif()
