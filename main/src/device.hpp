#pragma once
#include "config.hpp"
#include "raylib.h"
#include "rlgl.h"
#include "external/msf_gif.h"
#include <cstdio>

struct ImGuiContext;

// ============================================================================
// DEVICE (Singleton) — raylib backend
// ============================================================================
 

class Device
{
private:
    int m_width;
    int m_height;
    bool m_running;
    bool m_ready;
    bool m_initialized;
    bool m_resize;
    bool m_imguiInitialized;
    bool m_gifRecording;
    int m_gifWidth;
    int m_gifHeight;
    int m_gifFrameCentis;
    int m_gifMaxBitDepth;
    int m_gifSampleEvery;
    int m_gifSampleCounter;
    FILE *m_gifFile;
    MsfGifState m_gifState;
    unsigned char *m_gifPixels;
    size_t m_gifPixelBytes;
    char m_gifPath[512];

 
    // Timing
    float m_deltaTime;
    unsigned int m_fps;

    Device();
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    void ResetGifCaptureState();
    bool CaptureGifFrame();
    void MakeGifCapturePath(char *buffer, size_t bufferSize) const;

public:
    static Device& Instance();

    /// Initialize raylib, create window and GL context
    bool Init(const char* title, int width, int height, unsigned int flags = 0);

    /// Shutdown everything
    void Close();

    bool InitImGui();
    void ShutdownImGui();

    /// Poll events, update delta time. Returns false if should quit.
    bool Running();

     
    void Flip();
    bool StartGifCapture(const char *path = nullptr);
    bool StopGifCapture();
    bool ToggleGifCapture(const char *path = nullptr);

    
    /// Run the main loop using the current stage. Returns when done.
    void Run();

    /// Request exit
    void Quit();

    bool IsResized() const { return m_resize; }

    // Getters
    int GetWidth()  const { return m_width; }
    int GetHeight() const { return m_height; }
    float GetDeltaTime() const { return m_deltaTime; }
    unsigned int GetFPS() const { return m_fps; }
    bool IsReady() const { return m_ready; }
    bool HasImGui() const { return m_imguiInitialized; }
    bool IsGifRecording() const { return m_gifRecording; }

 
    void SetTitle(const char* title);

 
    void SetSize(int width, int height);
};
