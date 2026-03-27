#include "bindings.hpp"
#include "device.hpp"
#include "raylib.h"
#include "gl_headers.h"
#include "utils.hpp"
#include "rlImGui.h"

namespace Bindings
{
    // ========== WINDOW / CORE (raylib-style API) ==========

    static int native_InitWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("InitWindow expects (width, height, title)");
            return 0;
        }
        int w = (int)args[0].asNumber();
        int h = (int)args[1].asNumber();
        const char *title = args[2].isString() ? args[2].asStringChars() : "BuGL";
        Device::Instance().Init(title, w, h, 0);
        return 0;
    }

    static int native_CloseWindow_(Interpreter *vm, int argc, Value *args)
    {
        (void)vm; (void)args;
        Device::Instance().Close();
        return 0;
    }

    static int native_WindowShouldClose(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushBool(::WindowShouldClose());
        return 1;
    }

    static int native_IsWindowReady_(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushBool(::IsWindowReady());
        return 1;
    }

    static int native_IsWindowResized_(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushBool(::IsWindowResized());
        return 1;
    }

    static int native_SetWindowTitle_(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 1 || !args[0].isString()) { Error("SetWindowTitle expects (title)"); return 0; }
        ::SetWindowTitle(args[0].asStringChars());
        return 0;
    }

    static int native_SetWindowSize_(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 2) { Error("SetWindowSize expects (w, h)"); return 0; }
        ::SetWindowSize((int)args[0].asNumber(), (int)args[1].asNumber());
        return 0;
    }

    static int native_GetScreenWidth_(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushInt(::GetScreenWidth());
        return 1;
    }

    static int native_GetScreenHeight_(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushInt(::GetScreenHeight());
        return 1;
    }

    static int native_SetTargetFPS_(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 1) { Error("SetTargetFPS expects (fps)"); return 0; }
        ::SetTargetFPS((int)args[0].asNumber());
        return 0;
    }

    static int native_GetFPS_(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushInt(::GetFPS());
        return 1;
    }

    static int native_GetFrameTime_(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushDouble(::GetFrameTime());
        return 1;
    }

    static int native_GetTime_(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushDouble(::GetTime());
        return 1;
    }

    static int native_SetConfigFlags_(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 1) { Error("SetConfigFlags expects (flags)"); return 0; }
        ::SetConfigFlags((unsigned int)args[0].asNumber());
        return 0;
    }

    // ========== DRAWING ==========

    static int native_BeginDrawing_(Interpreter *vm, int argc, Value *args)
    {
        (void)vm; (void)args;
        ::BeginDrawing();
        return 0;
    }

    static int native_EndDrawing_(Interpreter *vm, int argc, Value *args)
    {
        (void)vm; (void)args;
        ::EndDrawing();
        return 0;
    }

    static int native_BeginImGui(Interpreter *vm, int argc, Value *args)
    {
        (void)vm; (void)args;
        if (Device::Instance().HasImGui())
            rlImGuiBegin();
        return 0;
    }

    static int native_EndImGui(Interpreter *vm, int argc, Value *args)
    {
        (void)vm; (void)args;
        if (Device::Instance().HasImGui())
            rlImGuiEnd();
        return 0;
    }

    static int native_ClearBackground_(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 4) { Error("ClearBackground expects (r, g, b, a)"); return 0; }
        Color c;
        c.r = (unsigned char)args[0].asNumber();
        c.g = (unsigned char)args[1].asNumber();
        c.b = (unsigned char)args[2].asNumber();
        c.a = (unsigned char)args[3].asNumber();
        ::ClearBackground(c);
        return 0;
    }

    // ========== IMGUI (engine-specific) ==========

    static int native_InitImGui(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushBool(Device::Instance().InitImGui());
        return 1;
    }

    static int native_ShutdownImGui(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        Device::Instance().ShutdownImGui();
        return 0;
    }

    static int native_HasImGui(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushBool(Device::Instance().HasImGui());
        return 1;
    }

    // ========== GIF CAPTURE (engine-specific) ==========

    static int native_StartGifCapture(Interpreter *vm, int argc, Value *args)
    {
        const char *path = (argc >= 1 && args[0].isString()) ? args[0].asStringChars() : nullptr;
        vm->pushBool(Device::Instance().StartGifCapture(path));
        return 1;
    }

    static int native_StopGifCapture(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushBool(Device::Instance().StopGifCapture());
        return 1;
    }

    static int native_ToggleGifCapture(Interpreter *vm, int argc, Value *args)
    {
        const char *path = (argc >= 1 && args[0].isString()) ? args[0].asStringChars() : nullptr;
        vm->pushBool(Device::Instance().ToggleGifCapture(path));
        return 1;
    }

    static int native_IsGifRecording(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushBool(Device::Instance().IsGifRecording());
        return 1;
    }

    // ========== INPUT (direct raylib calls) ==========

    static int native_Input_IsMousePressed(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_IsMousePressed expects 1 numeric button");
            return 0;
        }
        vm->pushBool(IsMouseButtonPressed((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_IsMouseDown(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_IsMouseDown expects 1 numeric button");
            return 0;
        }
        vm->pushBool(IsMouseButtonDown((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_IsMouseReleased(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_IsMouseReleased expects 1 numeric button");
            return 0;
        }
        vm->pushBool(IsMouseButtonReleased((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_IsMouseUp(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_IsMouseUp expects 1 numeric button");
            return 0;
        }
        vm->pushBool(IsMouseButtonUp((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_GetMousePosition(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        if (argc != 0)
        {
            Error("Input_GetMousePosition expects 0 arguments");
            vm->pushDouble(0); vm->pushDouble(0);
            return 2;
        }
        Vector2 v = ::GetMousePosition();
        vm->pushDouble(v.x);
        vm->pushDouble(v.y);
        return 2;
    }

    static int native_Input_GetMouseDelta(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        if (argc != 0)
        {
            Error("Input_GetMouseDelta expects 0 arguments");
            vm->pushDouble(0); vm->pushDouble(0);
            return 2;
        }
        Vector2 v = ::GetMouseDelta();
        vm->pushDouble(v.x);
        vm->pushDouble(v.y);
        return 2;
    }

    static int native_Input_GetMouseX(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        if (argc != 0)
        {
            Error("Input_GetMouseX expects 0 arguments");
            return 0;
        }
        vm->pushInt(::GetMouseX());
        return 1;
    }

    static int native_Input_GetMouseY(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        if (argc != 0)
        {
            Error("Input_GetMouseY expects 0 arguments");
            return 0;
        }
        vm->pushInt(::GetMouseY());
        return 1;
    }

    static int native_Input_SetMousePosition(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 2 || !args[0].isNumber() || !args[1].isNumber())
        {
            Error("Input_SetMousePosition expects (x, y)");
            return 0;
        }
        ::SetMousePosition((int)args[0].asNumber(), (int)args[1].asNumber());
        return 0;
    }

    static int native_Input_SetMouseOffset(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 2 || !args[0].isNumber() || !args[1].isNumber())
        {
            Error("Input_SetMouseOffset expects (offsetX, offsetY)");
            return 0;
        }
        ::SetMouseOffset((int)args[0].asNumber(), (int)args[1].asNumber());
        return 0;
    }

    static int native_Input_SetMouseScale(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 2 || !args[0].isNumber() || !args[1].isNumber())
        {
            Error("Input_SetMouseScale expects (scaleX, scaleY)");
            return 0;
        }
        ::SetMouseScale((float)args[0].asNumber(), (float)args[1].asNumber());
        return 0;
    }

    static int native_Input_GetMouseWheelMove(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->pushDouble(::GetMouseWheelMove());
        return 1;
    }

    static int native_Input_GetMouseWheelMoveV(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        Vector2 v = ::GetMouseWheelMoveV();
        vm->pushDouble(v.x);
        vm->pushDouble(v.y);
        return 2;
    }

    static int native_Input_SetMouseCursor(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_SetMouseCursor expects 1 numeric cursor");
            return 0;
        }
        ::SetMouseCursor((int)args[0].asNumber());
        return 0;
    }

    static int native_Input_IsKeyPressed(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_IsKeyPressed expects 1 numeric key");
            return 0;
        }
        vm->pushBool(::IsKeyPressed((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_IsKeyDown(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_IsKeyDown expects 1 numeric key");
            return 0;
        }
        vm->pushBool(::IsKeyDown((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_IsKeyReleased(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_IsKeyReleased expects 1 numeric key");
            return 0;
        }
        vm->pushBool(::IsKeyReleased((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_IsKeyUp(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_IsKeyUp expects 1 numeric key");
            return 0;
        }
        vm->pushBool(::IsKeyUp((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_GetKeyPressed(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        if (argc != 0)
        {
            Error("Input_GetKeyPressed expects 0 arguments");
            return 0;
        }
        vm->pushInt(::GetKeyPressed());
        return 1;
    }

    static int native_Input_GetCharPressed(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        if (argc != 0)
        {
            Error("Input_GetCharPressed expects 0 arguments");
            return 0;
        }
        vm->pushInt(::GetCharPressed());
        return 1;
    }

    static int native_Input_IsGamepadAvailable(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_IsGamepadAvailable expects 1 numeric gamepad");
            return 0;
        }
        vm->pushBool(::IsGamepadAvailable((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_GetGamepadName(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_GetGamepadName expects 1 numeric gamepad");
            return 0;
        }
        vm->push(vm->makeString(::GetGamepadName((int)args[0].asNumber())));
        return 1;
    }

    static int native_Input_IsGamepadButtonPressed(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNumber() || !args[1].isNumber())
        {
            Error("Input_IsGamepadButtonPressed expects (gamepad, button)");
            return 0;
        }
        vm->pushBool(::IsGamepadButtonPressed((int)args[0].asNumber(), (int)args[1].asNumber()));
        return 1;
    }

    static int native_Input_IsGamepadButtonDown(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNumber() || !args[1].isNumber())
        {
            Error("Input_IsGamepadButtonDown expects (gamepad, button)");
            return 0;
        }
        vm->pushBool(::IsGamepadButtonDown((int)args[0].asNumber(), (int)args[1].asNumber()));
        return 1;
    }

    static int native_Input_IsGamepadButtonReleased(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNumber() || !args[1].isNumber())
        {
            Error("Input_IsGamepadButtonReleased expects (gamepad, button)");
            return 0;
        }
        vm->pushBool(::IsGamepadButtonReleased((int)args[0].asNumber(), (int)args[1].asNumber()));
        return 1;
    }

    static int native_Input_IsGamepadButtonUp(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNumber() || !args[1].isNumber())
        {
            Error("Input_IsGamepadButtonUp expects (gamepad, button)");
            return 0;
        }
        vm->pushBool(::IsGamepadButtonUp((int)args[0].asNumber(), (int)args[1].asNumber()));
        return 1;
    }

    static int native_Input_GetGamepadButtonPressed(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        if (argc != 0)
        {
            Error("Input_GetGamepadButtonPressed expects 0 arguments");
            return 0;
        }
        // Check all gamepads for a pressed button
        for (int g = 0; g < 4; g++)
        {
            if (!::IsGamepadAvailable(g)) continue;
            for (int b = 0; b < 18; b++)
            {
                if (::IsGamepadButtonPressed(g, b))
                {
                    vm->pushInt(b);
                    return 1;
                }
            }
        }
        vm->pushInt(0);
        return 1;
    }

    static int native_Input_GetGamepadAxisCount(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNumber())
        {
            Error("Input_GetGamepadAxisCount expects 1 numeric gamepad");
            return 0;
        }
        vm->pushInt(::GetGamepadAxisCount((int)args[0].asNumber()));
        return 1;
    }

    static int native_Input_GetGamepadAxisMovement(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNumber() || !args[1].isNumber())
        {
            Error("Input_GetGamepadAxisMovement expects (gamepad, axis)");
            return 0;
        }
        vm->pushDouble(::GetGamepadAxisMovement((int)args[0].asNumber(), (int)args[1].asNumber()));
        return 1;
    }

    // ========================================
    // TOUCH INPUT
    // ========================================

    static int native_Input_GetTouchX(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(::GetTouchX());
        return 1;
    }

    static int native_Input_GetTouchY(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(::GetTouchY());
        return 1;
    }

    static int native_Input_GetTouchPosition(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        Vector2 pos = ::GetTouchPosition(args[0].asInt());
        vm->pushFloat(pos.x);
        vm->pushFloat(pos.y);
        return 2;
    }

    static int native_Input_GetTouchPointCount(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(::GetTouchPointCount());
        return 1;
    }

    static int native_Input_GetTouchPointId(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        vm->pushInt(::GetTouchPointId(args[0].asInt()));
        return 1;
    }

    // ========================================
    // GESTURE INPUT
    // ========================================

    static int native_Input_SetGesturesEnabled(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        ::SetGesturesEnabled((unsigned int)args[0].asInt());
        return 0;
    }

    static int native_Input_IsGestureDetected(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        vm->pushBool(::IsGestureDetected((unsigned int)args[0].asInt()));
        return 1;
    }

    static int native_Input_GetGestureDetected(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(::GetGestureDetected());
        return 1;
    }

    static int native_Input_GetGestureHoldDuration(Interpreter *vm, int argc, Value *args)
    {
        vm->pushFloat(::GetGestureHoldDuration());
        return 1;
    }

    static int native_Input_GetGestureDragVector(Interpreter *vm, int argc, Value *args)
    {
        Vector2 v = ::GetGestureDragVector();
        vm->pushFloat(v.x);
        vm->pushFloat(v.y);
        return 2;
    }

    static int native_Input_GetGestureDragAngle(Interpreter *vm, int argc, Value *args)
    {
        vm->pushFloat(::GetGestureDragAngle());
        return 1;
    }

    static int native_Input_GetGesturePinchVector(Interpreter *vm, int argc, Value *args)
    {
        Vector2 v = ::GetGesturePinchVector();
        vm->pushFloat(v.x);
        vm->pushFloat(v.y);
        return 2;
    }

    static int native_Input_GetGesturePinchAngle(Interpreter *vm, int argc, Value *args)
    {
        vm->pushFloat(::GetGesturePinchAngle());
        return 1;
    }

    static int native_Input_IsKeyPressedRepeat(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        vm->pushBool(::IsKeyPressedRepeat(args[0].asInt()));
        return 1;
    }

    void register_device_input(Interpreter &vm)
    {
        // Window/drawing/timing now registered in ray/core.cpp
        vm.registerNative("SetConfigFlags", native_SetConfigFlags_, 1);
        vm.registerNative("InitImGui", native_InitImGui, 0);
        vm.registerNative("ShutdownImGui", native_ShutdownImGui, 0);
        vm.registerNative("HasImGui", native_HasImGui, 0);
        vm.registerNative("BeginImGui", native_BeginImGui, 0);
        vm.registerNative("EndImGui", native_EndImGui, 0);
        vm.registerNative("StartGifCapture", native_StartGifCapture, -1);
        vm.registerNative("StopGifCapture", native_StopGifCapture, 0);
        vm.registerNative("ToggleGifCapture", native_ToggleGifCapture, -1);
        vm.registerNative("IsGifRecording", native_IsGifRecording, 0);

        vm.registerNative("IsMouseButtonPressed", native_Input_IsMousePressed, 1);
        vm.registerNative("IsMouseButtonDown", native_Input_IsMouseDown, 1);
        vm.registerNative("IsMouseButtonReleased", native_Input_IsMouseReleased, 1);
        vm.registerNative("IsMouseButtonUp", native_Input_IsMouseUp, 1);
        vm.registerNative("GetMousePosition", native_Input_GetMousePosition, 0);
        vm.registerNative("GetMouseDelta", native_Input_GetMouseDelta, 0);
        vm.registerNative("GetMouseX", native_Input_GetMouseX, 0);
        vm.registerNative("GetMouseY", native_Input_GetMouseY, 0);
        vm.registerNative("SetMousePosition", native_Input_SetMousePosition, 2);
        vm.registerNative("SetMouseOffset", native_Input_SetMouseOffset, 2);
        vm.registerNative("SetMouseScale", native_Input_SetMouseScale, 2);
        vm.registerNative("GetMouseWheelMove", native_Input_GetMouseWheelMove, 0);
        vm.registerNative("GetMouseWheelMoveV", native_Input_GetMouseWheelMoveV, 0);
        vm.registerNative("SetMouseCursor", native_Input_SetMouseCursor, 1);
        vm.registerNative("IsKeyPressed", native_Input_IsKeyPressed, 1);
        vm.registerNative("IsKeyDown", native_Input_IsKeyDown, 1);
        vm.registerNative("IsKeyReleased", native_Input_IsKeyReleased, 1);
        vm.registerNative("IsKeyUp", native_Input_IsKeyUp, 1);
        vm.registerNative("GetKeyPressed", native_Input_GetKeyPressed, 0);
        vm.registerNative("GetCharPressed", native_Input_GetCharPressed, 0);
        vm.registerNative("IsGamepadAvailable", native_Input_IsGamepadAvailable, 1);
        vm.registerNative("GetGamepadName", native_Input_GetGamepadName, 1);
        vm.registerNative("IsGamepadButtonPressed", native_Input_IsGamepadButtonPressed, 2);
        vm.registerNative("IsGamepadButtonDown", native_Input_IsGamepadButtonDown, 2);
        vm.registerNative("IsGamepadButtonReleased", native_Input_IsGamepadButtonReleased, 2);
        vm.registerNative("IsGamepadButtonUp", native_Input_IsGamepadButtonUp, 2);
        vm.registerNative("GetGamepadButtonPressed", native_Input_GetGamepadButtonPressed, 0);
        vm.registerNative("GetGamepadAxisCount", native_Input_GetGamepadAxisCount, 1);
        vm.registerNative("GetGamepadAxisMovement", native_Input_GetGamepadAxisMovement, 2);

        vm.addGlobal("MOUSE_BUTTON_LEFT", vm.makeInt(MOUSE_BUTTON_LEFT));
        vm.addGlobal("MOUSE_BUTTON_RIGHT", vm.makeInt(MOUSE_BUTTON_RIGHT));
        vm.addGlobal("MOUSE_BUTTON_MIDDLE", vm.makeInt(MOUSE_BUTTON_MIDDLE));
        vm.addGlobal("MOUSE_CURSOR_DEFAULT", vm.makeInt(MOUSE_CURSOR_DEFAULT));
        vm.addGlobal("MOUSE_CURSOR_ARROW", vm.makeInt(MOUSE_CURSOR_ARROW));
        vm.addGlobal("MOUSE_CURSOR_IBEAM", vm.makeInt(MOUSE_CURSOR_IBEAM));
        vm.addGlobal("MOUSE_CURSOR_CROSSHAIR", vm.makeInt(MOUSE_CURSOR_CROSSHAIR));
        vm.addGlobal("MOUSE_CURSOR_POINTING_HAND", vm.makeInt(MOUSE_CURSOR_POINTING_HAND));
        vm.addGlobal("MOUSE_CURSOR_RESIZE_EW", vm.makeInt(MOUSE_CURSOR_RESIZE_EW));
        vm.addGlobal("MOUSE_CURSOR_RESIZE_NS", vm.makeInt(MOUSE_CURSOR_RESIZE_NS));
        vm.addGlobal("MOUSE_CURSOR_RESIZE_NWSE", vm.makeInt(MOUSE_CURSOR_RESIZE_NWSE));
        vm.addGlobal("MOUSE_CURSOR_RESIZE_NESW", vm.makeInt(MOUSE_CURSOR_RESIZE_NESW));
        vm.addGlobal("MOUSE_CURSOR_RESIZE_ALL", vm.makeInt(MOUSE_CURSOR_RESIZE_ALL));
        vm.addGlobal("MOUSE_CURSOR_NOT_ALLOWED", vm.makeInt(MOUSE_CURSOR_NOT_ALLOWED));
        vm.addGlobal("KEY_NULL", vm.makeInt(KEY_NULL));
        vm.addGlobal("KEY_APOSTROPHE", vm.makeInt(KEY_APOSTROPHE));
        vm.addGlobal("KEY_COMMA", vm.makeInt(KEY_COMMA));
        vm.addGlobal("KEY_MINUS", vm.makeInt(KEY_MINUS));
        vm.addGlobal("KEY_PERIOD", vm.makeInt(KEY_PERIOD));
        vm.addGlobal("KEY_SLASH", vm.makeInt(KEY_SLASH));
        vm.addGlobal("KEY_ZERO", vm.makeInt(KEY_ZERO));
        vm.addGlobal("KEY_ONE", vm.makeInt(KEY_ONE));
        vm.addGlobal("KEY_TWO", vm.makeInt(KEY_TWO));
        vm.addGlobal("KEY_THREE", vm.makeInt(KEY_THREE));
        vm.addGlobal("KEY_FOUR", vm.makeInt(KEY_FOUR));
        vm.addGlobal("KEY_FIVE", vm.makeInt(KEY_FIVE));
        vm.addGlobal("KEY_SIX", vm.makeInt(KEY_SIX));
        vm.addGlobal("KEY_SEVEN", vm.makeInt(KEY_SEVEN));
        vm.addGlobal("KEY_EIGHT", vm.makeInt(KEY_EIGHT));
        vm.addGlobal("KEY_NINE", vm.makeInt(KEY_NINE));
        vm.addGlobal("KEY_SEMICOLON", vm.makeInt(KEY_SEMICOLON));
        vm.addGlobal("KEY_EQUAL", vm.makeInt(KEY_EQUAL));
        vm.addGlobal("KEY_A", vm.makeInt(KEY_A));
        vm.addGlobal("KEY_B", vm.makeInt(KEY_B));
        vm.addGlobal("KEY_C", vm.makeInt(KEY_C));
        vm.addGlobal("KEY_D", vm.makeInt(KEY_D));
        vm.addGlobal("KEY_E", vm.makeInt(KEY_E));
        vm.addGlobal("KEY_F", vm.makeInt(KEY_F));
        vm.addGlobal("KEY_G", vm.makeInt(KEY_G));
        vm.addGlobal("KEY_H", vm.makeInt(KEY_H));
        vm.addGlobal("KEY_I", vm.makeInt(KEY_I));
        vm.addGlobal("KEY_J", vm.makeInt(KEY_J));
        vm.addGlobal("KEY_K", vm.makeInt(KEY_K));
        vm.addGlobal("KEY_L", vm.makeInt(KEY_L));
        vm.addGlobal("KEY_M", vm.makeInt(KEY_M));
        vm.addGlobal("KEY_N", vm.makeInt(KEY_N));
        vm.addGlobal("KEY_O", vm.makeInt(KEY_O));
        vm.addGlobal("KEY_P", vm.makeInt(KEY_P));
        vm.addGlobal("KEY_Q", vm.makeInt(KEY_Q));
        vm.addGlobal("KEY_R", vm.makeInt(KEY_R));
        vm.addGlobal("KEY_S", vm.makeInt(KEY_S));
        vm.addGlobal("KEY_T", vm.makeInt(KEY_T));
        vm.addGlobal("KEY_U", vm.makeInt(KEY_U));
        vm.addGlobal("KEY_V", vm.makeInt(KEY_V));
        vm.addGlobal("KEY_W", vm.makeInt(KEY_W));
        vm.addGlobal("KEY_X", vm.makeInt(KEY_X));
        vm.addGlobal("KEY_Y", vm.makeInt(KEY_Y));
        vm.addGlobal("KEY_Z", vm.makeInt(KEY_Z));
        vm.addGlobal("KEY_LEFT_BRACKET", vm.makeInt(KEY_LEFT_BRACKET));
        vm.addGlobal("KEY_BACKSLASH", vm.makeInt(KEY_BACKSLASH));
        vm.addGlobal("KEY_RIGHT_BRACKET", vm.makeInt(KEY_RIGHT_BRACKET));
        vm.addGlobal("KEY_GRAVE", vm.makeInt(KEY_GRAVE));
        vm.addGlobal("KEY_SPACE", vm.makeInt(KEY_SPACE));
        vm.addGlobal("KEY_ESCAPE", vm.makeInt(KEY_ESCAPE));
        vm.addGlobal("KEY_ENTER", vm.makeInt(KEY_ENTER));
        vm.addGlobal("KEY_TAB", vm.makeInt(KEY_TAB));
        vm.addGlobal("KEY_BACKSPACE", vm.makeInt(KEY_BACKSPACE));
        vm.addGlobal("KEY_INSERT", vm.makeInt(KEY_INSERT));
        vm.addGlobal("KEY_DELETE", vm.makeInt(KEY_DELETE));
        vm.addGlobal("KEY_RIGHT", vm.makeInt(KEY_RIGHT));
        vm.addGlobal("KEY_LEFT", vm.makeInt(KEY_LEFT));
        vm.addGlobal("KEY_DOWN", vm.makeInt(KEY_DOWN));
        vm.addGlobal("KEY_UP", vm.makeInt(KEY_UP));
        vm.addGlobal("KEY_PAGE_UP", vm.makeInt(KEY_PAGE_UP));
        vm.addGlobal("KEY_PAGE_DOWN", vm.makeInt(KEY_PAGE_DOWN));
        vm.addGlobal("KEY_HOME", vm.makeInt(KEY_HOME));
        vm.addGlobal("KEY_END", vm.makeInt(KEY_END));
        vm.addGlobal("KEY_CAPS_LOCK", vm.makeInt(KEY_CAPS_LOCK));
        vm.addGlobal("KEY_SCROLL_LOCK", vm.makeInt(KEY_SCROLL_LOCK));
        vm.addGlobal("KEY_NUM_LOCK", vm.makeInt(KEY_NUM_LOCK));
        vm.addGlobal("KEY_PRINT_SCREEN", vm.makeInt(KEY_PRINT_SCREEN));
        vm.addGlobal("KEY_PAUSE", vm.makeInt(KEY_PAUSE));
        vm.addGlobal("KEY_F1", vm.makeInt(KEY_F1));
        vm.addGlobal("KEY_F2", vm.makeInt(KEY_F2));
        vm.addGlobal("KEY_F3", vm.makeInt(KEY_F3));
        vm.addGlobal("KEY_F4", vm.makeInt(KEY_F4));
        vm.addGlobal("KEY_F5", vm.makeInt(KEY_F5));
        vm.addGlobal("KEY_F6", vm.makeInt(KEY_F6));
        vm.addGlobal("KEY_F7", vm.makeInt(KEY_F7));
        vm.addGlobal("KEY_F8", vm.makeInt(KEY_F8));
        vm.addGlobal("KEY_F9", vm.makeInt(KEY_F9));
        vm.addGlobal("KEY_F10", vm.makeInt(KEY_F10));
        vm.addGlobal("KEY_F11", vm.makeInt(KEY_F11));
        vm.addGlobal("KEY_F12", vm.makeInt(KEY_F12));
        vm.addGlobal("KEY_KP_0", vm.makeInt(KEY_KP_0));
        vm.addGlobal("KEY_KP_1", vm.makeInt(KEY_KP_1));
        vm.addGlobal("KEY_KP_2", vm.makeInt(KEY_KP_2));
        vm.addGlobal("KEY_KP_3", vm.makeInt(KEY_KP_3));
        vm.addGlobal("KEY_KP_4", vm.makeInt(KEY_KP_4));
        vm.addGlobal("KEY_KP_5", vm.makeInt(KEY_KP_5));
        vm.addGlobal("KEY_KP_6", vm.makeInt(KEY_KP_6));
        vm.addGlobal("KEY_KP_7", vm.makeInt(KEY_KP_7));
        vm.addGlobal("KEY_KP_8", vm.makeInt(KEY_KP_8));
        vm.addGlobal("KEY_KP_9", vm.makeInt(KEY_KP_9));
        vm.addGlobal("KEY_KP_DECIMAL", vm.makeInt(KEY_KP_DECIMAL));
        vm.addGlobal("KEY_KP_DIVIDE", vm.makeInt(KEY_KP_DIVIDE));
        vm.addGlobal("KEY_KP_MULTIPLY", vm.makeInt(KEY_KP_MULTIPLY));
        vm.addGlobal("KEY_KP_SUBTRACT", vm.makeInt(KEY_KP_SUBTRACT));
        vm.addGlobal("KEY_KP_ADD", vm.makeInt(KEY_KP_ADD));
        vm.addGlobal("KEY_KP_ENTER", vm.makeInt(KEY_KP_ENTER));
        vm.addGlobal("KEY_KP_EQUAL", vm.makeInt(KEY_KP_EQUAL));
        vm.addGlobal("KEY_LEFT_SHIFT", vm.makeInt(KEY_LEFT_SHIFT));
        vm.addGlobal("KEY_LEFT_CONTROL", vm.makeInt(KEY_LEFT_CONTROL));
        vm.addGlobal("KEY_LEFT_ALT", vm.makeInt(KEY_LEFT_ALT));
        vm.addGlobal("KEY_LEFT_SUPER", vm.makeInt(KEY_LEFT_SUPER));
        vm.addGlobal("KEY_RIGHT_SHIFT", vm.makeInt(KEY_RIGHT_SHIFT));
        vm.addGlobal("KEY_RIGHT_CONTROL", vm.makeInt(KEY_RIGHT_CONTROL));
        vm.addGlobal("KEY_RIGHT_ALT", vm.makeInt(KEY_RIGHT_ALT));
        vm.addGlobal("KEY_RIGHT_SUPER", vm.makeInt(KEY_RIGHT_SUPER));
        vm.addGlobal("KEY_KB_MENU", vm.makeInt(KEY_KB_MENU));
        vm.addGlobal("GAMEPAD_BUTTON_UNKNOWN", vm.makeInt(GAMEPAD_BUTTON_UNKNOWN));
        vm.addGlobal("GAMEPAD_BUTTON_LEFT_FACE_UP", vm.makeInt(GAMEPAD_BUTTON_LEFT_FACE_UP));
        vm.addGlobal("GAMEPAD_BUTTON_LEFT_FACE_RIGHT", vm.makeInt(GAMEPAD_BUTTON_LEFT_FACE_RIGHT));
        vm.addGlobal("GAMEPAD_BUTTON_LEFT_FACE_DOWN", vm.makeInt(GAMEPAD_BUTTON_LEFT_FACE_DOWN));
        vm.addGlobal("GAMEPAD_BUTTON_LEFT_FACE_LEFT", vm.makeInt(GAMEPAD_BUTTON_LEFT_FACE_LEFT));
        vm.addGlobal("GAMEPAD_BUTTON_RIGHT_FACE_UP", vm.makeInt(GAMEPAD_BUTTON_RIGHT_FACE_UP));
        vm.addGlobal("GAMEPAD_BUTTON_RIGHT_FACE_RIGHT", vm.makeInt(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT));
        vm.addGlobal("GAMEPAD_BUTTON_RIGHT_FACE_DOWN", vm.makeInt(GAMEPAD_BUTTON_RIGHT_FACE_DOWN));
        vm.addGlobal("GAMEPAD_BUTTON_RIGHT_FACE_LEFT", vm.makeInt(GAMEPAD_BUTTON_RIGHT_FACE_LEFT));
        vm.addGlobal("GAMEPAD_BUTTON_LEFT_TRIGGER_1", vm.makeInt(GAMEPAD_BUTTON_LEFT_TRIGGER_1));
        vm.addGlobal("GAMEPAD_BUTTON_LEFT_TRIGGER_2", vm.makeInt(GAMEPAD_BUTTON_LEFT_TRIGGER_2));
        vm.addGlobal("GAMEPAD_BUTTON_RIGHT_TRIGGER_1", vm.makeInt(GAMEPAD_BUTTON_RIGHT_TRIGGER_1));
        vm.addGlobal("GAMEPAD_BUTTON_RIGHT_TRIGGER_2", vm.makeInt(GAMEPAD_BUTTON_RIGHT_TRIGGER_2));
        vm.addGlobal("GAMEPAD_BUTTON_MIDDLE_LEFT", vm.makeInt(GAMEPAD_BUTTON_MIDDLE_LEFT));
        vm.addGlobal("GAMEPAD_BUTTON_MIDDLE", vm.makeInt(GAMEPAD_BUTTON_MIDDLE));
        vm.addGlobal("GAMEPAD_BUTTON_MIDDLE_RIGHT", vm.makeInt(GAMEPAD_BUTTON_MIDDLE_RIGHT));
        vm.addGlobal("GAMEPAD_BUTTON_LEFT_THUMB", vm.makeInt(GAMEPAD_BUTTON_LEFT_THUMB));
        vm.addGlobal("GAMEPAD_BUTTON_RIGHT_THUMB", vm.makeInt(GAMEPAD_BUTTON_RIGHT_THUMB));
        vm.addGlobal("GAMEPAD_AXIS_LEFT_X", vm.makeInt(GAMEPAD_AXIS_LEFT_X));
        vm.addGlobal("GAMEPAD_AXIS_LEFT_Y", vm.makeInt(GAMEPAD_AXIS_LEFT_Y));
        vm.addGlobal("GAMEPAD_AXIS_RIGHT_X", vm.makeInt(GAMEPAD_AXIS_RIGHT_X));
        vm.addGlobal("GAMEPAD_AXIS_RIGHT_Y", vm.makeInt(GAMEPAD_AXIS_RIGHT_Y));
        vm.addGlobal("GAMEPAD_AXIS_LEFT_TRIGGER", vm.makeInt(GAMEPAD_AXIS_LEFT_TRIGGER));
        vm.addGlobal("GAMEPAD_AXIS_RIGHT_TRIGGER", vm.makeInt(GAMEPAD_AXIS_RIGHT_TRIGGER));

        // ConfigFlags
        vm.addGlobal("FLAG_VSYNC_HINT", vm.makeInt(FLAG_VSYNC_HINT));
        vm.addGlobal("FLAG_FULLSCREEN_MODE", vm.makeInt(FLAG_FULLSCREEN_MODE));
        vm.addGlobal("FLAG_WINDOW_RESIZABLE", vm.makeInt(FLAG_WINDOW_RESIZABLE));
        vm.addGlobal("FLAG_WINDOW_UNDECORATED", vm.makeInt(FLAG_WINDOW_UNDECORATED));
        vm.addGlobal("FLAG_WINDOW_HIDDEN", vm.makeInt(FLAG_WINDOW_HIDDEN));
        vm.addGlobal("FLAG_WINDOW_MINIMIZED", vm.makeInt(FLAG_WINDOW_MINIMIZED));
        vm.addGlobal("FLAG_WINDOW_MAXIMIZED", vm.makeInt(FLAG_WINDOW_MAXIMIZED));
        vm.addGlobal("FLAG_WINDOW_UNFOCUSED", vm.makeInt(FLAG_WINDOW_UNFOCUSED));
        vm.addGlobal("FLAG_WINDOW_TOPMOST", vm.makeInt(FLAG_WINDOW_TOPMOST));
        vm.addGlobal("FLAG_WINDOW_ALWAYS_RUN", vm.makeInt(FLAG_WINDOW_ALWAYS_RUN));
        vm.addGlobal("FLAG_WINDOW_TRANSPARENT", vm.makeInt(FLAG_WINDOW_TRANSPARENT));
        vm.addGlobal("FLAG_WINDOW_HIGHDPI", vm.makeInt(FLAG_WINDOW_HIGHDPI));
        vm.addGlobal("FLAG_WINDOW_MOUSE_PASSTHROUGH", vm.makeInt(FLAG_WINDOW_MOUSE_PASSTHROUGH));
        vm.addGlobal("FLAG_MSAA_4X_HINT", vm.makeInt(FLAG_MSAA_4X_HINT));
        vm.addGlobal("FLAG_INTERLACED_HINT", vm.makeInt(FLAG_INTERLACED_HINT));

        // Touch input
        vm.registerNative("GetTouchX", native_Input_GetTouchX, 0);
        vm.registerNative("GetTouchY", native_Input_GetTouchY, 0);
        vm.registerNative("GetTouchPosition", native_Input_GetTouchPosition, 1);
        vm.registerNative("GetTouchPointCount", native_Input_GetTouchPointCount, 0);
        vm.registerNative("GetTouchPointId", native_Input_GetTouchPointId, 1);

        // Gesture input
        vm.registerNative("SetGesturesEnabled", native_Input_SetGesturesEnabled, 1);
        vm.registerNative("IsGestureDetected", native_Input_IsGestureDetected, 1);
        vm.registerNative("GetGestureDetected", native_Input_GetGestureDetected, 0);
        vm.registerNative("GetGestureHoldDuration", native_Input_GetGestureHoldDuration, 0);
        vm.registerNative("GetGestureDragVector", native_Input_GetGestureDragVector, 0);
        vm.registerNative("GetGestureDragAngle", native_Input_GetGestureDragAngle, 0);
        vm.registerNative("GetGesturePinchVector", native_Input_GetGesturePinchVector, 0);
        vm.registerNative("GetGesturePinchAngle", native_Input_GetGesturePinchAngle, 0);
        vm.registerNative("IsKeyPressedRepeat", native_Input_IsKeyPressedRepeat, 1);

        // Gesture constants
        vm.addGlobal("GESTURE_NONE", vm.makeInt(GESTURE_NONE));
        vm.addGlobal("GESTURE_TAP", vm.makeInt(GESTURE_TAP));
        vm.addGlobal("GESTURE_DOUBLETAP", vm.makeInt(GESTURE_DOUBLETAP));
        vm.addGlobal("GESTURE_HOLD", vm.makeInt(GESTURE_HOLD));
        vm.addGlobal("GESTURE_DRAG", vm.makeInt(GESTURE_DRAG));
        vm.addGlobal("GESTURE_SWIPE_RIGHT", vm.makeInt(GESTURE_SWIPE_RIGHT));
        vm.addGlobal("GESTURE_SWIPE_LEFT", vm.makeInt(GESTURE_SWIPE_LEFT));
        vm.addGlobal("GESTURE_SWIPE_UP", vm.makeInt(GESTURE_SWIPE_UP));
        vm.addGlobal("GESTURE_SWIPE_DOWN", vm.makeInt(GESTURE_SWIPE_DOWN));
        vm.addGlobal("GESTURE_PINCH_IN", vm.makeInt(GESTURE_PINCH_IN));
        vm.addGlobal("GESTURE_PINCH_OUT", vm.makeInt(GESTURE_PINCH_OUT));
    }
}
