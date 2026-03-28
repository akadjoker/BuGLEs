cmake_minimum_required(VERSION 3.20)

project(editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
option(BUGL_EDITOR_EMBEDDED_VM "Enable in-process embedded VM inside editor (requires runtime wiring)" OFF)

set(EDITOR_SOURCES
    src/main.cpp
    src/rlImGui.cpp
    src/TextEditor.cpp
    src/LanguageDefinitions.cpp
    src/DocumentSymbols.cpp
    src/GifRecorder.cpp
    src/ImGuiConsole.cpp
    src/ImGuiDebugPanel.cpp
    src/ImGuiFileDialog.cpp
    src/ImGuiPopup.cpp
    src/EmbeddedVM.cpp
)

# Vendored boost regex sources
file(GLOB BOOST_REGEX_SOURCES CONFIGURE_DEPENDS "src/vendor/regex/src/*.cpp")

add_executable(editor ${EDITOR_SOURCES} ${BOOST_REGEX_SOURCES})

target_include_directories(editor PRIVATE
    src
    src/vendor
    src/vendor/regex/include
    ${CMAKE_SOURCE_DIR}/vendor/imgui
    ${CMAKE_SOURCE_DIR}/vendor/raylib/src
    ${CMAKE_SOURCE_DIR}/vendor/raylib/src/external
)

target_compile_definitions(editor PRIVATE
    BUGL_EDITOR_EMBEDDED_VM=$<BOOL:${BUGL_EDITOR_EMBEDDED_VM}>
)

if(BUGL_EDITOR_EMBEDDED_VM)
    message(FATAL_ERROR "BUGL_EDITOR_EMBEDDED_VM=ON is not wired in this repository yet. Keep it OFF to run scripts via external bugl binary.")
endif()

# External-runtime mode: editor launches scripts through the bugl executable.
target_link_libraries(editor PRIVATE imgui raylib)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if(UNIX)
        target_compile_options(editor PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -D_DEBUG -DVERBOSE)
        target_link_options(editor PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -D_DEBUG)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(editor PRIVATE -O3 -DNDEBUG)
endif()

if(WIN32)
    target_link_libraries(editor PRIVATE Winmm.lib)
endif()

if(UNIX)
    target_link_libraries(editor PRIVATE m)
endif()
