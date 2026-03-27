#if !defined(GRAPHICS_API_OPENGL_ES2) && !defined(GRAPHICS_API_OPENGL_ES3)
#include "bindings.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#include "rlgl.h"
#include "gl_headers.h"

namespace Bindings
{
    static int native_glGetUniformBlockIndex(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[1].isString())
        {
            Error("glGetUniformBlockIndex expects (program, blockName)");
            return 0;
        }
        GLuint idx = glGetUniformBlockIndex((GLuint)args[0].asNumber(), args[1].asStringChars());
#ifdef GL_INVALID_INDEX
        if (idx == GL_INVALID_INDEX)
            vm->pushInt(-1);
        else
            vm->pushInt((int)idx);
#else
        if (idx == 0xFFFFFFFFu)
            vm->pushInt(-1);
        else
            vm->pushInt((int)idx);
#endif
        return 1;
    }

    static int native_glUniformBlockBinding(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 3)
        {
            Error("glUniformBlockBinding expects (program, blockIndex, bindingPoint)");
            return 0;
        }
        glUniformBlockBinding((GLuint)args[0].asNumber(),
                              (GLuint)args[1].asNumber(),
                              (GLuint)args[2].asNumber());
        return 0;
    }

    static int native_glBindBufferBase(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 3)
        {
            Error("glBindBufferBase expects (target, index, buffer)");
            return 0;
        }
        glBindBufferBase((GLenum)args[0].asNumber(),
                         (GLuint)args[1].asNumber(),
                         (GLuint)args[2].asNumber());
        return 0;
    }

    static int native_glBindBufferRange(Interpreter *vm, int argc, Value *args)
    {
        (void)vm;
        if (argc != 5)
        {
            Error("glBindBufferRange expects (target, index, buffer, offset, size)");
            return 0;
        }
        glBindBufferRange((GLenum)args[0].asNumber(),
                          (GLuint)args[1].asNumber(),
                          (GLuint)args[2].asNumber(),
                          (GLintptr)args[3].asNumber(),
                          (GLsizeiptr)args[4].asNumber());
        return 0;
    }

    void register_opengl_ubo(ModuleBuilder &module)
    {
        module.addFunction("glGetUniformBlockIndex", native_glGetUniformBlockIndex, 2)
            .addFunction("glUniformBlockBinding", native_glUniformBlockBinding, 3)
            .addFunction("glBindBufferBase", native_glBindBufferBase, 3)
            .addFunction("glBindBufferRange", native_glBindBufferRange, 5);

#ifdef GL_UNIFORM_BUFFER
        module.addInt("GL_UNIFORM_BUFFER", GL_UNIFORM_BUFFER);
#endif
#ifdef GL_UNIFORM_BUFFER_BINDING
        module.addInt("GL_UNIFORM_BUFFER_BINDING", GL_UNIFORM_BUFFER_BINDING);
#endif
#ifdef GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
        module.addInt("GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT", GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT);
#endif
#ifdef GL_MAX_UNIFORM_BUFFER_BINDINGS
        module.addInt("GL_MAX_UNIFORM_BUFFER_BINDINGS", GL_MAX_UNIFORM_BUFFER_BINDINGS);
#endif
#ifdef GL_MAX_VERTEX_UNIFORM_BLOCKS
        module.addInt("GL_MAX_VERTEX_UNIFORM_BLOCKS", GL_MAX_VERTEX_UNIFORM_BLOCKS);
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_BLOCKS
        module.addInt("GL_MAX_FRAGMENT_UNIFORM_BLOCKS", GL_MAX_FRAGMENT_UNIFORM_BLOCKS);
#endif
#ifdef GL_UNIFORM_BLOCK_DATA_SIZE
        module.addInt("GL_UNIFORM_BLOCK_DATA_SIZE", GL_UNIFORM_BLOCK_DATA_SIZE);
#endif
#ifdef GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS
        module.addInt("GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS", GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS);
#endif
#ifdef GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES
        module.addInt("GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES", GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES);
#endif
#ifdef GL_UNIFORM_BLOCK_BINDING
        module.addInt("GL_UNIFORM_BLOCK_BINDING", GL_UNIFORM_BLOCK_BINDING);
#endif
    }
}
#endif // !GRAPHICS_API_OPENGL_ES2 && !GRAPHICS_API_OPENGL_ES3
