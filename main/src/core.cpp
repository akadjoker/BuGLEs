#include "bindings.hpp"
#include "raylib.h"
#include <cstdio>
#include <thread>
#include <chrono>

namespace Bindings
{

    int native_Engine_Init(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        // raylib initialization is handled by Device::Init
        vm->push(vm->makeInt(0));
        return 1;
    }

    int native_Engine_Quit(Interpreter *vm, int argc, Value *args)
    {
        (void)vm; (void)args;
        return 0;
    }

    int native_Engine_GetError(Interpreter *vm, int argc, Value *args)
    {
        (void)args;
        vm->push(vm->makeString(""));
        return 1;
    }

    int native_Engine_Delay(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("Delay expects 1 argument");
            return 0;
        }

        unsigned int ms = (unsigned int)args[0].asNumber();
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return 0;
    }

    void register_core(ModuleBuilder &mod)
    {

        mod.addFunction("Init", native_Engine_Init, 1)
            .addFunction("Quit", native_Engine_Quit, 0)
            .addFunction("GetError", native_Engine_GetError, 0)
            .addFunction("Delay", native_Engine_Delay, 1);
    }
}