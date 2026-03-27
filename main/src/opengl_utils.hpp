#pragma once

#include "interpreter.hpp"
#include "rlgl.h"
#include <vector>
#include <cstdint>

namespace GLUtils
{
    inline bool resolveUploadData(const Value &value,
                                  size_t size,
                                  const char *fnName,
                                  const void **outPtr,
                                  std::vector<unsigned char> &scratch)
    {
        if (value.isNil())
        {
            *outPtr = nullptr;
            return true;
        }

        if (value.isPointer())
        {
            *outPtr = value.asPointer();
            return true;
        }

        if (value.isBuffer())
        {
            BufferInstance *buf = value.asBuffer();
            if (buf && size > 0)
            {
                size_t have = (size_t)buf->count * (size_t)buf->elementSize;
                if (have < size)
                    return false;
            }
            *outPtr = buf ? (const void *)buf->data : nullptr;
            return true;
        }

        if (getBuiltinTypedArrayData(value, outPtr))
            return true;

        if (value.isArray())
        {
            ArrayInstance *arr = value.asArray();
            if (!arr) return false;

            const int count = (int)arr->values.size();
            if (count <= 0 || size <= 0)
            {
                *outPtr = nullptr;
                return true;
            }

            if (size == (size_t)(count * (int)sizeof(float)))
            {
                scratch.resize((size_t)count * sizeof(float));
                float *out = (float *)scratch.data();
                for (int i = 0; i < count; i++)
                    out[i] = (float)arr->values[i].asNumber();
                *outPtr = scratch.data();
                return true;
            }

            if (size == (size_t)(count * (int)sizeof(int)))
            {
                scratch.resize((size_t)count * sizeof(int));
                int *out = (int *)scratch.data();
                for (int i = 0; i < count; i++)
                    out[i] = arr->values[i].asInt();
                *outPtr = scratch.data();
                return true;
            }

            return false;
        }

        return false;
    }
}
