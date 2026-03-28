#include "pch.h"
#include "device.hpp"
#include "utils.hpp"
#include "rlgl.h"
#include "gl_headers.h"
#include "imgui.h"
#include "rlImGui.h"
#include <ctime>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace
{
    static bool create_directory_single(const char *path)
    {
        if (!path || !path[0])
            return true;

#ifdef _WIN32
        if (_mkdir(path) == 0 || errno == EEXIST)
            return true;
#else
        if (mkdir(path, 0755) == 0 || errno == EEXIST)
            return true;
#endif
        return false;
    }

    static bool ensure_parent_directories(const char *path)
    {
        if (!path || !path[0])
            return false;

        char buffer[512];
        std::snprintf(buffer, sizeof(buffer), "%s", path);

        char *lastSlash = std::strrchr(buffer, '/');
#ifdef _WIN32
        char *lastBackslash = std::strrchr(buffer, '\\');
        if (!lastSlash || (lastBackslash && lastBackslash > lastSlash))
            lastSlash = lastBackslash;
#endif
        if (!lastSlash)
            return true;

        *lastSlash = '\0';
        if (!buffer[0])
            return true;

        for (char *cursor = buffer + 1; *cursor; ++cursor)
        {
            if (*cursor == '/' || *cursor == '\\')
            {
                char saved = *cursor;
                *cursor = '\0';
                if (buffer[0] && !create_directory_single(buffer))
                    return false;
                *cursor = saved;
            }
        }

        return create_directory_single(buffer);
    }
}
// ============================================================================
// DEVICE
// ============================================================================

Device::Device()
    : m_width(0), m_height(0), m_running(false), m_ready(false), m_initialized(false), m_resize(false), m_imguiInitialized(false),
      m_gifRecording(false), m_gifWidth(0), m_gifHeight(0), m_gifFrameCentis(5), m_gifMaxBitDepth(16), m_gifSampleEvery(2), m_gifSampleCounter(0),
      m_gifFile(nullptr), m_gifState({}), m_gifPixels(nullptr), m_gifPixelBytes(0), m_deltaTime(0.0f), m_fps(0)
{
    m_gifPath[0] = '\0';
}

Device::~Device()
{
    Close();
}

Device &Device::Instance()
{
    static Device instance;
    return instance;
}

bool Device::Init(const char *title, int width, int height, unsigned int flags)
{
    if (m_ready)
    {
        LogWarning(" Init - Already initialized\n");
        return true;
    }

    // ---- Raylib Window ----
    unsigned int raylibFlags = FLAG_WINDOW_HIGHDPI;
    if (flags & FLAG_WINDOW_RESIZABLE)
        raylibFlags |= FLAG_WINDOW_RESIZABLE;
    if (flags & FLAG_FULLSCREEN_MODE)
        raylibFlags |= FLAG_FULLSCREEN_MODE;

    SetConfigFlags(raylibFlags);
    InitWindow(width, height, title ? title : "Device");

    if (!IsWindowReady())
    {
        LogError("   InitWindow failed");
        return false;
    }

    m_width = width;
    m_height = height;

    // ---- Log GL info ----
    LogInfo("  Vendor:   %s", (const char*)glGetString(GL_VENDOR));
    LogInfo("  Renderer: %s", (const char*)glGetString(GL_RENDERER));
    LogInfo("  Version:  %s", (const char*)glGetString(GL_VERSION));
    LogInfo("  GLSL:     %s", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    m_running = true;
    m_ready = true;
    m_resize = false;
    m_initialized = true;

    return true;
}

void Device::Close()
{
    if (!m_ready)
    {
        if (IsWindowReady())
        {
            ::CloseWindow();
        }

        m_running = false;
        m_ready = false;
        m_initialized = false;
        return;
    }

    m_initialized = false;
    StopGifCapture();
    ShutdownImGui();

    if (IsWindowReady())
        ::CloseWindow();

    m_running = false;
    m_ready = false;
}

bool Device::InitImGui()
{
    if (!m_ready)
    {
        if (!IsWindowReady())
        {
            LogError(" InitImGui - Device is not initialized");
            return false;
        }

        // Allow ImGui on scripts that created the raylib window through the
        // raw InitWindow() binding instead of Device::Init().
        m_width = GetScreenWidth();
        m_height = GetScreenHeight();
        m_running = true;
        m_ready = true;
        m_initialized = true;
    }

    if (m_imguiInitialized)
        return true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!rlImGuiSetup(true))
    {
        LogError(" InitImGui - rlImGui setup failed");
        ImGui::DestroyContext();
        return false;
    }

    m_imguiInitialized = true;
    return true;
}

void Device::ShutdownImGui()
{
    if (!m_imguiInitialized)
        return;

    rlImGuiShutdown();
    ImGui::DestroyContext();
    m_imguiInitialized = false;
}

bool Device::Running()
{
    if (!m_running)
        return false;

    if (WindowShouldClose())
    {
        m_running = false;
        return false;
    }

    // ---- Delta time ----
    m_deltaTime = GetFrameTime();
    if (m_deltaTime > 0.1f)
        m_deltaTime = 0.1f; // clamp on long stalls

    // ---- FPS counter ----
    m_fps = GetFPS();

    // ---- Resize detection ----
    int newW = GetScreenWidth();
    int newH = GetScreenHeight();
    if (newW != m_width || newH != m_height)
    {
        m_width = newW;
        m_height = newH;
        m_resize = true;
    }

    if (m_imguiInitialized)
    {
        rlImGuiBegin();
    }

    // Begin raylib frame (sets up framebuffer, polls events).
    // Raw GL calls happen between BeginDrawing() and EndDrawing() in Flip().
    BeginDrawing();

    return m_running;
}

void Device::Flip()
{
    if (m_imguiInitialized)
    {
        rlImGuiEnd();
    }

    if (m_gifRecording)
        CaptureGifFrame();

    EndDrawing();
    m_resize = false;
}

void Device::Quit()
{
    m_running = false;
}

void Device::SetTitle(const char *title)
{
    if (m_ready && title)
        SetWindowTitle(title);
}

void Device::SetSize(int width, int height)
{
    if (m_ready)
    {
        SetWindowSize(width, height);
        m_width = width;
        m_height = height;
    }
}

void Device::ResetGifCaptureState()
{
    if (m_gifFile)
    {
        std::fclose(m_gifFile);
        m_gifFile = nullptr;
    }

    m_gifState = {};
    if (m_gifPixels)
    {
        std::free(m_gifPixels);
        m_gifPixels = nullptr;
    }
    m_gifPixelBytes = 0;
    m_gifPath[0] = '\0';
    m_gifRecording = false;
    m_gifWidth = 0;
    m_gifHeight = 0;
    m_gifSampleCounter = 0;
}

void Device::MakeGifCapturePath(char *buffer, size_t bufferSize) const
{
    if (!buffer || bufferSize == 0)
        return;

    std::time_t now = std::time(nullptr);
    std::tm tmValue = {};
#if defined(_WIN32)
    localtime_s(&tmValue, &now);
#else
    localtime_r(&now, &tmValue);
#endif

    char nameBuffer[64];
    strftime(nameBuffer, sizeof(nameBuffer), "%Y-%m-%d_%H-%M-%S.gif", &tmValue);
    std::snprintf(buffer, bufferSize, "gif/%s", nameBuffer);
}

bool Device::StartGifCapture(const char *path)
{
    if (!m_ready)
    {
        LogWarning(" GIF capture ignored: device not ready");
        return false;
    }

    if (m_gifRecording)
        return true;

    const int captureWidth = (m_width > 0) ? m_width : 1;
    const int captureHeight = (m_height > 0) ? m_height : 1;
    const size_t totalBytes = (size_t)captureWidth * (size_t)captureHeight * 4u;
    char outputPath[512];
    if (path && path[0] != '\0')
        std::snprintf(outputPath, sizeof(outputPath), "%s", path);
    else
        MakeGifCapturePath(outputPath, sizeof(outputPath));

    if (!ensure_parent_directories(outputPath))
    {
        LogError(" GIF capture failed to create directory for: %s", outputPath);
        return false;
    }

    FILE *file = std::fopen(outputPath, "wb");
    if (!file)
    {
        LogError(" GIF capture failed to open file: %s", outputPath);
        return false;
    }

    m_gifPixels = (unsigned char *)std::malloc(totalBytes);
    if (!m_gifPixels)
    {
        LogError(" GIF capture failed to allocate %zu bytes", totalBytes);
        std::fclose(file);
        return false;
    }
    std::memset(m_gifPixels, 0, totalBytes);
    m_gifPixelBytes = totalBytes;
    m_gifState = {};
    if (!msf_gif_begin_to_file(&m_gifState, captureWidth, captureHeight, (MsfGifFileWriteFunc)std::fwrite, (void *)file))
    {
        LogError(" GIF capture failed to begin encoder");
        std::fclose(file);
        std::free(m_gifPixels);
        m_gifPixels = nullptr;
        m_gifPixelBytes = 0;
        m_gifState = {};
        return false;
    }

    m_gifFile = file;
    std::snprintf(m_gifPath, sizeof(m_gifPath), "%s", outputPath);
    m_gifRecording = true;
    m_gifWidth = captureWidth;
    m_gifHeight = captureHeight;
    m_gifSampleCounter = 0;
    LogInfo(" GIF capture started: %s", m_gifPath);
    return true;
}

bool Device::StopGifCapture()
{
    if (!m_gifRecording)
        return false;

    const bool ok = msf_gif_end_to_file(&m_gifState) != 0;
    if (!ok)
        LogError(" GIF capture failed to finalize: %s", m_gifPath);

    char savedPath[512];
    std::snprintf(savedPath, sizeof(savedPath), "%s", m_gifPath);
    ResetGifCaptureState();
    if (ok)
        LogInfo(" GIF capture saved: %s", savedPath);
    return ok;
}

bool Device::ToggleGifCapture(const char *path)
{
    if (m_gifRecording)
        return StopGifCapture();
    return StartGifCapture(path);
}

bool Device::CaptureGifFrame()
{
    if (!m_gifRecording)
        return false;

    if (m_width != m_gifWidth || m_height != m_gifHeight)
    {
        LogWarning(" GIF capture stopped after resize: %dx%d -> %dx%d", m_gifWidth, m_gifHeight, m_width, m_height);
        StopGifCapture();
        return false;
    }

    m_gifSampleCounter++;
    if (m_gifSampleCounter < m_gifSampleEvery)
        return true;

    m_gifSampleCounter = 0;
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, m_gifWidth, m_gifHeight, GL_RGBA, GL_UNSIGNED_BYTE, m_gifPixels);

    if (!msf_gif_frame_to_file(&m_gifState, m_gifPixels, m_gifFrameCentis, m_gifMaxBitDepth, -m_gifWidth * 4))
    {
        LogError(" GIF capture failed to write frame: %s", m_gifPath);
        StopGifCapture();
        return false;
    }

    return true;
}
