#include "bindings_internal.hpp"

namespace ImGuiBindings
{
    bool sync_context(Interpreter *vm, const char *fn)
    {
        if (!ImGui::GetCurrentContext())
        {
            vm->runtimeError("%s requires InitImGui()", fn);
            return false;
        }
        return true;
    }

    bool ensure_context(Interpreter *vm, const char *fn)
    {
        return sync_context(vm, fn);
    }

    int push_nil(Interpreter *vm)
    {
        vm->push(vm->makeNil());
        return 1;
    }

    int push_nils(Interpreter *vm, int count)
    {
        for (int i = 0; i < count; ++i)
            vm->push(vm->makeNil());
        return count;
    }

    bool optional_number_arg(Value *args, int index, int argCount, float defaultValue, float *out)
    {
        if (index >= argCount)
        {
            *out = defaultValue;
            return true;
        }
        if (!args[index].isNumber())
            return false;
        *out = (float)args[index].asNumber();
        return true;
    }

    bool optional_int_arg(Value *args, int index, int argCount, int defaultValue, int *out)
    {
        if (index >= argCount)
        {
            *out = defaultValue;
            return true;
        }
        if (!args[index].isNumber())
            return false;
        *out = (int)args[index].asNumber();
        return true;
    }

    bool optional_string_arg(Value *args, int index, int argCount, const char *defaultValue, const char **out)
    {
        if (index >= argCount)
        {
            *out = defaultValue;
            return true;
        }
        if (!args[index].isString())
            return false;
        *out = args[index].asStringChars();
        return true;
    }
}
