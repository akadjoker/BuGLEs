#include "bindings.hpp"
#include "opengl_utils.hpp"
#include "gl_headers.h"

#include "rlgl.h"

namespace RLGLBindings
{
    static NativeStructDef *getMatrixDef(Interpreter *vm)
    {
        if (!vm) return nullptr;
        Value sv;
        if (!vm->tryGetGlobal("Matrix", &sv) || !sv.isNativeStruct()) return nullptr;
        Value iv = vm->createNativeStruct(sv.asNativeStructId(), 0, nullptr);
        NativeStructInstance *inst = iv.asNativeStructInstance();
        return inst ? inst->def : nullptr;
    }

    static bool valueToMatrix(Interpreter *vm, const Value &val, Matrix &out)
    {
        if (val.isNativeStructInstance())
        {
            NativeStructDef *matDef = getMatrixDef(vm);
            NativeStructInstance *inst = val.asNativeStructInstance();
            if (matDef && inst && inst->def == matDef && inst->data)
            {
                out = *(Matrix *)inst->data;
                return true;
            }
        }

        if (val.isArray())
        {
            ArrayInstance *a = val.asArray();
            if (!a || a->values.size() < 16) return false;
            float f[16];
            for (int i = 0; i < 16; i++) f[i] = (float)a->values[i].asNumber();
            out = {f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],
                   f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]};
            return true;
        }

        return false;
    }

    static bool pushMatrixNative(Interpreter *vm, Matrix m)
    {
        NativeStructDef *matDef = getMatrixDef(vm);
        if (!matDef) return false;
        Value out = vm->createNativeStruct(matDef->id, 0, nullptr);
        NativeStructInstance *inst = out.asNativeStructInstance();
        if (!inst || !inst->data) return false;
        *(Matrix *)inst->data = m;
        vm->push(out);
        return true;
    }



    int native_rlGetVersion(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(rlGetVersion());
        return 1;
    }

    int native_rlCheckErrors(Interpreter *vm, int argc, Value *args)
    {
        rlCheckErrors();
        return 0;
    }

    // =============================================================
    // 2. MATRIZES
    // =============================================================

    int native_rlMatrixMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        rlMatrixMode(args[0].asNumber());
        return 0;
    }

    int native_rlPushMatrix(Interpreter *vm, int argc, Value *args)
    {
        rlPushMatrix();
        return 0;
    }

    int native_rlPopMatrix(Interpreter *vm, int argc, Value *args)
    {
        rlPopMatrix();
        return 0;
    }

    int native_rlLoadIdentity(Interpreter *vm, int argc, Value *args)
    {
        rlLoadIdentity();
        return 0;
    }

    int native_rlTranslatef(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlTranslatef(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlRotatef(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlRotatef(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return 0;
    }

    int native_rlScalef(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlScalef(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlOrtho(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
            return 0;
        rlOrtho(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber(), args[5].asNumber());
        return 0;
    }

    int native_rlFrustum(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
            return 0;
        rlFrustum(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber(), args[5].asNumber());
        return 0;
    }

    int native_rlViewport(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlViewport(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return 0;
    }

    // =============================================================
    // 3. DESENHO (IMMEDIATE MODE)
    // =============================================================

    int native_rlBegin(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        rlBegin(args[0].asNumber());
        return 0;
    }

    int native_rlEnd(Interpreter *vm, int argc, Value *args)
    {
        rlEnd();
        return 0;
    }

    int native_rlVertex2f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        rlVertex2f(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    int native_rlVertex3f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlVertex3f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlTexCoord2f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        rlTexCoord2f(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    int native_rlNormal3f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlNormal3f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlColor4f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlColor4f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return 0;
    }

    int native_rlColor3f(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlColor3f(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    // =============================================================
    // 4. ESTADOS DE RENDERIZAÇÃO
    // =============================================================

    int native_rlEnableDepthTest(Interpreter *vm, int argc, Value *args)
    {
        rlEnableDepthTest();
        return 0;
    }
    int native_rlDisableDepthTest(Interpreter *vm, int argc, Value *args)
    {
        rlDisableDepthTest();
        return 0;
    }
    int native_rlEnableDepthMask(Interpreter *vm, int argc, Value *args)
    {
        rlEnableDepthMask();
        return 0;
    }
    int native_rlDisableDepthMask(Interpreter *vm, int argc, Value *args)
    {
        rlDisableDepthMask();
        return 0;
    }

    int native_rlEnableBackfaceCulling(Interpreter *vm, int argc, Value *args)
    {
        rlEnableBackfaceCulling();
        return 0;
    }
    int native_rlDisableBackfaceCulling(Interpreter *vm, int argc, Value *args)
    {
        rlDisableBackfaceCulling();
        return 0;
    }
    int native_rlSetCullFace(Interpreter *vm, int argc, Value *args)
    {
        rlSetCullFace(args[0].asNumber());
        return 0;
    }

    int native_rlEnableScissorTest(Interpreter *vm, int argc, Value *args)
    {
        rlEnableScissorTest();
        return 0;
    }
    int native_rlDisableScissorTest(Interpreter *vm, int argc, Value *args)
    {
        rlDisableScissorTest();
        return 0;
    }
    int native_rlScissor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlScissor(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber());
        return 0;
    }

    int native_rlEnableWireMode(Interpreter *vm, int argc, Value *args)
    {
        rlEnableWireMode();
        return 0;
    }
    int native_rlEnablePointMode(Interpreter *vm, int argc, Value *args)
    {
        rlEnablePointMode();
        return 0;
    }
    int native_rlDisableWireMode(Interpreter *vm, int argc, Value *args)
    {
        rlDisableWireMode();
        return 0;
    }

    int native_rlSetLineWidth(Interpreter *vm, int argc, Value *args)
    {
        rlSetLineWidth(args[0].asNumber());
        return 0;
    }

    int native_rlEnableColorBlend(Interpreter *vm, int argc, Value *args)
    {
        rlEnableColorBlend();
        return 0;
    }
    int native_rlDisableColorBlend(Interpreter *vm, int argc, Value *args)
    {
        rlDisableColorBlend();
        return 0;
    }
    int native_rlSetBlendMode(Interpreter *vm, int argc, Value *args)
    {
        rlSetBlendMode(args[0].asNumber());
        return 0;
    }

    int native_rlClearColor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        rlClearColor((uint8_t)args[0].asNumber(), (uint8_t)args[1].asNumber(), (uint8_t)args[2].asNumber(), (uint8_t)args[3].asNumber());
        return 0;
    }
    int native_rlClearScreenBuffers(Interpreter *vm, int argc, Value *args)
    {
        rlClearScreenBuffers();
        return 0;
    }

    // =============================================================
    // 5. VERTEX ARRAYS & BUFFERS
    // =============================================================

    int native_rlLoadVertexArray(Interpreter *vm, int argc, Value *args)
    {
         vm->pushInt(rlLoadVertexArray());
        return 1;
    }

    int native_rlLoadVertexBuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        // buffer (void*), size, dynamic
        void *ptr = nullptr;
        int size = args[1].asNumber();
        bool dynamic = args[2].asBool();
         vm->pushInt(rlLoadVertexBuffer(ptr, size, dynamic));
        return 1;
    }

    int native_rlEnableVertexArray(Interpreter *vm, int argc, Value *args)
    {
        rlEnableVertexArray(args[0].asNumber());
        return 0;
    }
    int native_rlDisableVertexArray(Interpreter *vm, int argc, Value *args)
    {
        rlDisableVertexArray();
        return 0;
    }

    int native_rlEnableVertexBuffer(Interpreter *vm, int argc, Value *args)
    {
        rlEnableVertexBuffer(args[0].asNumber());
        return 0;
    }
    int native_rlDisableVertexBuffer(Interpreter *vm, int argc, Value *args)
    {
        rlDisableVertexBuffer();
        return 0;
    }

    int native_rlUnloadVertexArray(Interpreter *vm, int argc, Value *args)
    {
        rlUnloadVertexArray(args[0].asNumber());
        return 0;
    }
    int native_rlUnloadVertexBuffer(Interpreter *vm, int argc, Value *args)
    {
        rlUnloadVertexBuffer(args[0].asNumber());
        return 0;
    }

    // =============================================================
    // 6. FRAMEBUFFERS & TEXTURAS
    // =============================================================

    int native_rlLoadFramebuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        vm->pushInt(rlLoadFramebuffer(args[0].asNumber(), args[1].asNumber()));
        return 1;
    }

    int native_rlFramebufferAttach(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
            return 0;
        rlFramebufferAttach(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber());
        return 0;
    }

    int native_rlFramebufferComplete(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        vm->pushBool(rlFramebufferComplete(args[0].asNumber()));
        return 1;
    }

    int native_rlUnloadFramebuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        rlUnloadFramebuffer(args[0].asNumber());
        return 0;
    }

    int native_rlEnableFramebuffer(Interpreter *vm, int argc, Value *args)
    {
        rlEnableFramebuffer(args[0].asNumber());
        return 0;
    }
    int native_rlDisableFramebuffer(Interpreter *vm, int argc, Value *args)
    {
        rlDisableFramebuffer();
        return 0;
    }

    int native_rlActiveTextureSlot(Interpreter *vm, int argc, Value *args)
    {
        rlActiveTextureSlot(args[0].asNumber());
        return 0;
    }
    int native_rlEnableTexture(Interpreter *vm, int argc, Value *args)
    {
        rlEnableTexture(args[0].asNumber());
        return 0;
    }
    int native_rlDisableTexture(Interpreter *vm, int argc, Value *args)
    {
        rlDisableTexture();
        return 0;
    }

    int native_rlTextureParameters(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlTextureParameters(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlGenTextureMipmaps(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
            return 0;
        int mipmaps = 1;
        rlGenTextureMipmaps(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), &mipmaps);
        vm->pushInt(mipmaps);
        return 1;
    }

    // =============================================================
    // 7. SHADERS (Vertex, Fragment, Compute)
    // =============================================================

    int native_rlLoadShaderCode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        const char *vs = args[0].isString() ? args[0].asStringChars() : nullptr;
        const char *fs = args[1].isString() ? args[1].asStringChars() : nullptr;
       vm->pushInt(rlLoadShaderCode(vs, fs));
        return 1;
    }

    int native_rlCompileShader(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        vm->pushInt(rlCompileShader(args[0].asStringChars(), args[1].asNumber()));
        return 1;
    }

    int native_rlLoadShaderProgram(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        vm->pushInt(rlLoadShaderProgram(args[0].asNumber(), args[1].asNumber()));
        return 1;
    }

    int native_rlUnloadShaderProgram(Interpreter *vm, int argc, Value *args)
    {
        rlUnloadShaderProgram(args[0].asNumber());
        return 0;
    }

    int native_rlEnableShader(Interpreter *vm, int argc, Value *args)
    {
        rlEnableShader(args[0].asNumber());
        return 0;
    }
    int native_rlDisableShader(Interpreter *vm, int argc, Value *args)
    {
        rlDisableShader();
        return 0;
    }

    int native_rlGetLocationUniform(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)            
        {
            return 0;
        }
        vm->pushInt(rlGetLocationUniform(args[0].asNumber(), args[1].asStringChars()));
        return 1;
    }

    int native_rlSetUniform(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 3)
            return 0;
        int loc = args[0].asNumber();
        int type = args[2].asNumber();

        if (type == RL_SHADER_UNIFORM_FLOAT)
        {
            float val = args[1].asNumber();
            rlSetUniform(loc, &val, type, 1);
        }
        else if (type == RL_SHADER_UNIFORM_INT)
        {
            int val = args[1].asNumber();
            rlSetUniform(loc, &val, type, 1);
        }
        else if (type == RL_SHADER_UNIFORM_VEC3)
        {
            float vec[3];
            if (vm->getVec3(args[1], vec))
            {
                rlSetUniform(loc, vec, type, 1);
            }
        }
        else if (type == RL_SHADER_UNIFORM_VEC2)
        {
            if (args[1].isArray())
            {
                ArrayInstance *a = args[1].asArray();
                if (a && a->values.size() >= 2)
                {
                    float vec[2] = {(float)a->values[0].asNumber(), (float)a->values[1].asNumber()};
                    rlSetUniform(loc, vec, type, 1);
                }
            }
        }
        else if (type == RL_SHADER_UNIFORM_VEC4)
        {
            float vec[4];
            if (vm->getVec4(args[1], vec))
            {
                rlSetUniform(loc, vec, type, 1);
            }
        }
        return 0;
    }

    int native_rlSetUniformSampler(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return 0;
        rlSetUniformSampler(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    int native_rlLoadComputeShaderProgram(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        vm->pushInt(rlLoadComputeShaderProgram(args[0].asNumber()));
        return 1;
    }

    int native_rlComputeShaderDispatch(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;
        rlComputeShaderDispatch(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    int native_rlDrawRenderBatchActive(Interpreter *vm, int argc, Value *args)
    {
        rlDrawRenderBatchActive();
        return 0;
    }

    // =============================================================
    // 8. MISSING FUNCTIONS (NEW)
    // =============================================================

    int native_rlMultMatrixf(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        Matrix m;
        if (!valueToMatrix(vm, args[0], m)) return 0;
        float mat[16] = {m.m0,m.m1,m.m2,m.m3,m.m4,m.m5,m.m6,m.m7,
                         m.m8,m.m9,m.m10,m.m11,m.m12,m.m13,m.m14,m.m15};
        rlMultMatrixf(mat);
        return 0;
    }

    int native_rlColor4ub(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) return 0;
        rlColor4ub((unsigned char)args[0].asNumber(), (unsigned char)args[1].asNumber(),
                   (unsigned char)args[2].asNumber(), (unsigned char)args[3].asNumber());
        return 0;
    }

    // --- Vertex attribute management ---

    int native_rlSetVertexAttribute(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5) return 0;
        rlSetVertexAttribute((unsigned int)args[0].asNumber(), (int)args[1].asNumber(),
                             (int)args[2].asNumber(), (bool)args[3].asBool(),
                             (int)args[4].asNumber(), 0);
        return 0;
    }

    int native_rlSetVertexAttributeDivisor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        rlSetVertexAttributeDivisor((unsigned int)args[0].asNumber(), (int)args[1].asNumber());
        return 0;
    }

    int native_rlEnableVertexAttribute(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        rlEnableVertexAttribute((unsigned int)args[0].asNumber());
        return 0;
    }

    int native_rlDisableVertexAttribute(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        rlDisableVertexAttribute((unsigned int)args[0].asNumber());
        return 0;
    }

    // --- Vertex/index buffer updates ---

    int native_rlLoadVertexBufferElement(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        // Can't easily pass raw pointer from script, but we expose the API shape
        void *ptr = nullptr;
        int size = (int)args[1].asNumber();
        bool dynamic = args[2].asBool();
        vm->pushInt(rlLoadVertexBufferElement(ptr, size, dynamic));
        return 1;
    }

    int native_rlEnableVertexBufferElement(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        rlEnableVertexBufferElement((unsigned int)args[0].asNumber());
        return 0;
    }

    int native_rlDisableVertexBufferElement(Interpreter *vm, int argc, Value *args)
    {
        rlDisableVertexBufferElement();
        return 0;
    }

    // --- Draw calls ---

    int native_rlDrawVertexArray(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        rlDrawVertexArray((int)args[0].asNumber(), (int)args[1].asNumber());
        return 0;
    }

    int native_rlDrawVertexArrayElements(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        rlDrawVertexArrayElements((int)args[0].asNumber(), (int)args[1].asNumber(), nullptr);
        return 0;
    }

    int native_rlDrawVertexArrayInstanced(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        rlDrawVertexArrayInstanced((int)args[0].asNumber(), (int)args[1].asNumber(), (int)args[2].asNumber());
        return 0;
    }

    int native_rlDrawVertexArrayElementsInstanced(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) return 0;
        rlDrawVertexArrayElementsInstanced((int)args[0].asNumber(), (int)args[1].asNumber(), nullptr, (int)args[3].asNumber());
        return 0;
    }

    // --- Shader attribute queries ---

    int native_rlGetLocationAttrib(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        vm->pushInt(rlGetLocationAttrib((unsigned int)args[0].asNumber(), args[1].asStringChars()));
        return 1;
    }

    // --- Framebuffer queries ---

    int native_rlGetFramebufferWidth(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(rlGetFramebufferWidth());
        return 1;
    }

    int native_rlGetFramebufferHeight(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(rlGetFramebufferHeight());
        return 1;
    }

    int native_rlBlitFramebuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 8) return 0;
        rlBlitFramebuffer((int)args[0].asNumber(), (int)args[1].asNumber(),
                          (int)args[2].asNumber(), (int)args[3].asNumber(),
                          (int)args[4].asNumber(), (int)args[5].asNumber(),
                          (int)args[6].asNumber(), (int)args[7].asNumber(),
                          0x00004000); // GL_COLOR_BUFFER_BIT
        return 0;
    }

    int native_rlActiveDrawBuffers(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        rlActiveDrawBuffers((int)args[0].asNumber());
        return 0;
    }

    // --- Texture operations ---

    int native_rlLoadTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5) return 0;
        // rlLoadTexture(data, width, height, format, mipmapCount)
        // data is nullptr from script (texture loaded from GPU side or generated)
        vm->pushInt(rlLoadTexture(nullptr, (int)args[1].asNumber(), (int)args[2].asNumber(),
                                  (int)args[3].asNumber(), (int)args[4].asNumber()));
        return 1;
    }

    int native_rlLoadTextureDepth(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        vm->pushInt(rlLoadTextureDepth((int)args[0].asNumber(), (int)args[1].asNumber(), args[2].asBool()));
        return 1;
    }

    int native_rlLoadTextureCubemap(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        vm->pushInt(rlLoadTextureCubemap(nullptr, (int)args[1].asNumber(), (int)args[2].asNumber()));
        return 1;
    }

    int native_rlUnloadTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        rlUnloadTexture((unsigned int)args[0].asNumber());
        return 0;
    }

    int native_rlEnableTextureCubemap(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        rlEnableTextureCubemap((unsigned int)args[0].asNumber());
        return 0;
    }

    int native_rlDisableTextureCubemap(Interpreter *vm, int argc, Value *args)
    {
        rlDisableTextureCubemap();
        return 0;
    }

    int native_rlGetTextureIdDefault(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(rlGetTextureIdDefault());
        return 1;
    }

    int native_rlGetShaderIdDefault(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(rlGetShaderIdDefault());
        return 1;
    }

    int native_rlSetTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        rlSetTexture((unsigned int)args[0].asNumber());
        return 0;
    }

    int native_rlCheckRenderBatchLimit(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        vm->pushBool(rlCheckRenderBatchLimit((int)args[0].asNumber()));
        return 1;
    }

    // --- Blend modes (advanced) ---

    int native_rlSetBlendFactors(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        rlSetBlendFactors((int)args[0].asNumber(), (int)args[1].asNumber(), (int)args[2].asNumber());
        return 0;
    }

    int native_rlSetBlendFactorsSeparate(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6) return 0;
        rlSetBlendFactorsSeparate((int)args[0].asNumber(), (int)args[1].asNumber(),
                                  (int)args[2].asNumber(), (int)args[3].asNumber(),
                                  (int)args[4].asNumber(), (int)args[5].asNumber());
        return 0;
    }

    // --- Line rendering ---

    int native_rlEnableSmoothLines(Interpreter *vm, int argc, Value *args)
    {
        rlEnableSmoothLines();
        return 0;
    }

    int native_rlDisableSmoothLines(Interpreter *vm, int argc, Value *args)
    {
        rlDisableSmoothLines();
        return 0;
    }

    int native_rlGetLineWidth(Interpreter *vm, int argc, Value *args)
    {
        vm->pushFloat(rlGetLineWidth());
        return 1;
    }

    // --- Stereo rendering ---

    int native_rlEnableStereoRender(Interpreter *vm, int argc, Value *args)
    {
        rlEnableStereoRender();
        return 0;
    }

    int native_rlDisableStereoRender(Interpreter *vm, int argc, Value *args)
    {
        rlDisableStereoRender();
        return 0;
    }

    int native_rlIsStereoRenderEnabled(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(rlIsStereoRenderEnabled());
        return 1;
    }

    // --- Matrix state queries ---

    int native_rlSetMatrixModelview(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        Matrix m;
        if (!valueToMatrix(vm, args[0], m)) return 0;
        rlSetMatrixModelview(m);
        return 0;
    }

    int native_rlSetMatrixProjection(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        Matrix m;
        if (!valueToMatrix(vm, args[0], m)) return 0;
        rlSetMatrixProjection(m);
        return 0;
    }

    int native_rlGetMatrixModelview(Interpreter *vm, int argc, Value *args)
    {
        return pushMatrixNative(vm, rlGetMatrixModelview()) ? 1 : 0;
    }

    int native_rlGetMatrixProjection(Interpreter *vm, int argc, Value *args)
    {
        return pushMatrixNative(vm, rlGetMatrixProjection()) ? 1 : 0;
    }

    int native_rlGetMatrixTransform(Interpreter *vm, int argc, Value *args)
    {
        return pushMatrixNative(vm, rlGetMatrixTransform()) ? 1 : 0;
    }

    // --- Shader buffer objects (SSBO - compute) ---

    int native_rlLoadShaderBuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        vm->pushInt(rlLoadShaderBuffer((unsigned int)args[0].asNumber(), nullptr, (int)args[1].asNumber()));
        return 1;
    }

    int native_rlUnloadShaderBuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        rlUnloadShaderBuffer((unsigned int)args[0].asNumber());
        return 0;
    }

    int native_rlBindShaderBuffer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        rlBindShaderBuffer((unsigned int)args[0].asNumber(), (unsigned int)args[1].asNumber());
        return 0;
    }

    int native_rlGetShaderBufferSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        vm->pushInt((int)rlGetShaderBufferSize((unsigned int)args[0].asNumber()));
        return 1;
    }

    int native_rlBindImageTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) return 0;
        rlBindImageTexture((unsigned int)args[0].asNumber(), (unsigned int)args[1].asNumber(),
                           (int)args[2].asNumber(), args[3].asBool());
        return 0;
    }

    int native_rlSetUniformMatrix(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) return 0;
        int loc = (int)args[0].asNumber();
        int count = (int)args[1].asNumber();
        bool transpose = args[2].asBool();
        if (count <= 0) return 0;

        const void *ptr = nullptr;
        std::vector<unsigned char> scratch;
        size_t bytes = (size_t)count * 16 * sizeof(float);
        if (!GLUtils::resolveUploadData(args[3], bytes, "rlSetUniformMatrix", &ptr, scratch))
            return 0;

        glUniformMatrix4fv(loc, count, transpose ? GL_TRUE : GL_FALSE, (const float *)ptr);
        return 0;
    }

    // =============================================================
    // REGISTO FINAL
    // =============================================================

    void register_rlgl(Interpreter &vm)
    {
        ModuleBuilder module = vm.addModule("RLGL");

        // --- LIFECYCLE ---
        module.addFunction("rlGetVersion", native_rlGetVersion, 0)
              .addFunction("rlCheckErrors", native_rlCheckErrors, 0)

        // --- MATRIX ---
              .addFunction("rlMatrixMode", native_rlMatrixMode, 1)
              .addFunction("rlPushMatrix", native_rlPushMatrix, 0)
              .addFunction("rlPopMatrix", native_rlPopMatrix, 0)
              .addFunction("rlLoadIdentity", native_rlLoadIdentity, 0)
              .addFunction("rlTranslatef", native_rlTranslatef, 3)
              .addFunction("rlRotatef", native_rlRotatef, 4)
              .addFunction("rlScalef", native_rlScalef, 3)
              .addFunction("rlMultMatrixf", native_rlMultMatrixf, 1)
              .addFunction("rlOrtho", native_rlOrtho, 6)
              .addFunction("rlFrustum", native_rlFrustum, 6)
              .addFunction("rlViewport", native_rlViewport, 4)

        // --- IMMEDIATE MODE ---
              .addFunction("rlBegin", native_rlBegin, 1)
              .addFunction("rlEnd", native_rlEnd, 0)
              .addFunction("rlVertex2f", native_rlVertex2f, 2)
              .addFunction("rlVertex3f", native_rlVertex3f, 3)
              .addFunction("rlTexCoord2f", native_rlTexCoord2f, 2)
              .addFunction("rlNormal3f", native_rlNormal3f, 3)
              .addFunction("rlColor4f", native_rlColor4f, 4)
              .addFunction("rlColor3f", native_rlColor3f, 3)
              .addFunction("rlColor4ub", native_rlColor4ub, 4)

        // --- RENDER STATES ---
              .addFunction("rlEnableDepthTest", native_rlEnableDepthTest, 0)
              .addFunction("rlDisableDepthTest", native_rlDisableDepthTest, 0)
              .addFunction("rlEnableDepthMask", native_rlEnableDepthMask, 0)
              .addFunction("rlDisableDepthMask", native_rlDisableDepthMask, 0)
              .addFunction("rlEnableBackfaceCulling", native_rlEnableBackfaceCulling, 0)
              .addFunction("rlDisableBackfaceCulling", native_rlDisableBackfaceCulling, 0)
              .addFunction("rlSetCullFace", native_rlSetCullFace, 1)
              .addFunction("rlEnableScissorTest", native_rlEnableScissorTest, 0)
              .addFunction("rlDisableScissorTest", native_rlDisableScissorTest, 0)
              .addFunction("rlScissor", native_rlScissor, 4)
              .addFunction("rlEnableWireMode", native_rlEnableWireMode, 0)
              .addFunction("rlEnablePointMode", native_rlEnablePointMode, 0)
              .addFunction("rlDisableWireMode", native_rlDisableWireMode, 0)
              .addFunction("rlSetLineWidth", native_rlSetLineWidth, 1)
              .addFunction("rlGetLineWidth", native_rlGetLineWidth, 0)
              .addFunction("rlEnableSmoothLines", native_rlEnableSmoothLines, 0)
              .addFunction("rlDisableSmoothLines", native_rlDisableSmoothLines, 0)
              .addFunction("rlEnableColorBlend", native_rlEnableColorBlend, 0)
              .addFunction("rlDisableColorBlend", native_rlDisableColorBlend, 0)
              .addFunction("rlSetBlendMode", native_rlSetBlendMode, 1)
              .addFunction("rlSetBlendFactors", native_rlSetBlendFactors, 3)
              .addFunction("rlSetBlendFactorsSeparate", native_rlSetBlendFactorsSeparate, 6)
              .addFunction("rlClearColor", native_rlClearColor, 4)
              .addFunction("rlClearScreenBuffers", native_rlClearScreenBuffers, 0)
              .addFunction("rlDrawRenderBatchActive", native_rlDrawRenderBatchActive, 0)
              .addFunction("rlCheckRenderBatchLimit", native_rlCheckRenderBatchLimit, 1)
              .addFunction("rlSetTexture", native_rlSetTexture, 1)

        // --- VERTEX ARRAYS & BUFFERS ---
              .addFunction("rlLoadVertexArray", native_rlLoadVertexArray, 0)
              .addFunction("rlLoadVertexBuffer", native_rlLoadVertexBuffer, 3)
              .addFunction("rlLoadVertexBufferElement", native_rlLoadVertexBufferElement, 3)
              .addFunction("rlEnableVertexArray", native_rlEnableVertexArray, 1)
              .addFunction("rlDisableVertexArray", native_rlDisableVertexArray, 0)
              .addFunction("rlEnableVertexBuffer", native_rlEnableVertexBuffer, 1)
              .addFunction("rlDisableVertexBuffer", native_rlDisableVertexBuffer, 0)
              .addFunction("rlEnableVertexBufferElement", native_rlEnableVertexBufferElement, 1)
              .addFunction("rlDisableVertexBufferElement", native_rlDisableVertexBufferElement, 0)
              .addFunction("rlUnloadVertexArray", native_rlUnloadVertexArray, 1)
              .addFunction("rlUnloadVertexBuffer", native_rlUnloadVertexBuffer, 1)
              .addFunction("rlSetVertexAttribute", native_rlSetVertexAttribute, 5)
              .addFunction("rlSetVertexAttributeDivisor", native_rlSetVertexAttributeDivisor, 2)
              .addFunction("rlEnableVertexAttribute", native_rlEnableVertexAttribute, 1)
              .addFunction("rlDisableVertexAttribute", native_rlDisableVertexAttribute, 1)

        // --- DRAW CALLS ---
              .addFunction("rlDrawVertexArray", native_rlDrawVertexArray, 2)
              .addFunction("rlDrawVertexArrayElements", native_rlDrawVertexArrayElements, 3)
              .addFunction("rlDrawVertexArrayInstanced", native_rlDrawVertexArrayInstanced, 3)
              .addFunction("rlDrawVertexArrayElementsInstanced", native_rlDrawVertexArrayElementsInstanced, 4)

        // --- FRAMEBUFFERS ---
              .addFunction("rlLoadFramebuffer", native_rlLoadFramebuffer, 2)
              .addFunction("rlFramebufferAttach", native_rlFramebufferAttach, 5)
              .addFunction("rlFramebufferComplete", native_rlFramebufferComplete, 1)
              .addFunction("rlUnloadFramebuffer", native_rlUnloadFramebuffer, 1)
              .addFunction("rlEnableFramebuffer", native_rlEnableFramebuffer, 1)
              .addFunction("rlDisableFramebuffer", native_rlDisableFramebuffer, 0)
              .addFunction("rlGetFramebufferWidth", native_rlGetFramebufferWidth, 0)
              .addFunction("rlGetFramebufferHeight", native_rlGetFramebufferHeight, 0)
              .addFunction("rlBlitFramebuffer", native_rlBlitFramebuffer, 8)
              .addFunction("rlActiveDrawBuffers", native_rlActiveDrawBuffers, 1)

        // --- TEXTURES ---
              .addFunction("rlActiveTextureSlot", native_rlActiveTextureSlot, 1)
              .addFunction("rlEnableTexture", native_rlEnableTexture, 1)
              .addFunction("rlDisableTexture", native_rlDisableTexture, 0)
              .addFunction("rlEnableTextureCubemap", native_rlEnableTextureCubemap, 1)
              .addFunction("rlDisableTextureCubemap", native_rlDisableTextureCubemap, 0)
              .addFunction("rlTextureParameters", native_rlTextureParameters, 3)
              .addFunction("rlGenTextureMipmaps", native_rlGenTextureMipmaps, 4)
              .addFunction("rlLoadTexture", native_rlLoadTexture, 5)
              .addFunction("rlLoadTextureDepth", native_rlLoadTextureDepth, 3)
              .addFunction("rlLoadTextureCubemap", native_rlLoadTextureCubemap, 3)
              .addFunction("rlUnloadTexture", native_rlUnloadTexture, 1)
              .addFunction("rlGetTextureIdDefault", native_rlGetTextureIdDefault, 0)
              .addFunction("rlGetShaderIdDefault", native_rlGetShaderIdDefault, 0)

        // --- SHADERS ---
              .addFunction("rlLoadShaderCode", native_rlLoadShaderCode, 2)
              .addFunction("rlCompileShader", native_rlCompileShader, 2)
              .addFunction("rlLoadShaderProgram", native_rlLoadShaderProgram, 2)
              .addFunction("rlUnloadShaderProgram", native_rlUnloadShaderProgram, 1)
              .addFunction("rlEnableShader", native_rlEnableShader, 1)
              .addFunction("rlDisableShader", native_rlDisableShader, 0)
              .addFunction("rlGetLocationUniform", native_rlGetLocationUniform, 2)
              .addFunction("rlGetLocationAttrib", native_rlGetLocationAttrib, 2)
              .addFunction("rlSetUniform", native_rlSetUniform, 4)
              .addFunction("rlSetUniformMatrix", native_rlSetUniformMatrix, 4)
              .addFunction("rlSetUniformSampler", native_rlSetUniformSampler, 2)
              .addFunction("rlLoadComputeShaderProgram", native_rlLoadComputeShaderProgram, 1)
              .addFunction("rlComputeShaderDispatch", native_rlComputeShaderDispatch, 3)

        // --- SHADER BUFFER OBJECTS (SSBO) ---
              .addFunction("rlLoadShaderBuffer", native_rlLoadShaderBuffer, 2)
              .addFunction("rlUnloadShaderBuffer", native_rlUnloadShaderBuffer, 1)
              .addFunction("rlBindShaderBuffer", native_rlBindShaderBuffer, 2)
              .addFunction("rlGetShaderBufferSize", native_rlGetShaderBufferSize, 1)
              .addFunction("rlBindImageTexture", native_rlBindImageTexture, 4)

        // --- MATRIX STATE ---
              .addFunction("rlGetMatrixModelview", native_rlGetMatrixModelview, 0)
              .addFunction("rlGetMatrixProjection", native_rlGetMatrixProjection, 0)
              .addFunction("rlGetMatrixTransform", native_rlGetMatrixTransform, 0)
              .addFunction("rlSetMatrixModelview", native_rlSetMatrixModelview, 1)
              .addFunction("rlSetMatrixProjection", native_rlSetMatrixProjection, 1)

        // --- STEREO ---
              .addFunction("rlEnableStereoRender", native_rlEnableStereoRender, 0)
              .addFunction("rlDisableStereoRender", native_rlDisableStereoRender, 0)
              .addFunction("rlIsStereoRenderEnabled", native_rlIsStereoRenderEnabled, 0);

        // --- CONSTANTS ---

        // Primitives
        module.addInt("RL_LINES", RL_LINES)
              .addInt("RL_TRIANGLES", RL_TRIANGLES)
              .addInt("RL_QUADS", RL_QUADS)

        // Matrix Modes
              .addInt("RL_MODELVIEW", RL_MODELVIEW)
              .addInt("RL_PROJECTION", RL_PROJECTION)
              .addInt("RL_TEXTURE", RL_TEXTURE)

        // Culling
              .addInt("RL_CULL_FACE_FRONT", RL_CULL_FACE_FRONT)
              .addInt("RL_CULL_FACE_BACK", RL_CULL_FACE_BACK)

        // Texture Parameters
              .addInt("RL_TEXTURE_WRAP_S", RL_TEXTURE_WRAP_S)
              .addInt("RL_TEXTURE_WRAP_T", RL_TEXTURE_WRAP_T)
              .addInt("RL_TEXTURE_MAG_FILTER", RL_TEXTURE_MAG_FILTER)
              .addInt("RL_TEXTURE_MIN_FILTER", RL_TEXTURE_MIN_FILTER)
              .addInt("RL_TEXTURE_FILTER_NEAREST", RL_TEXTURE_FILTER_NEAREST)
              .addInt("RL_TEXTURE_FILTER_LINEAR", RL_TEXTURE_FILTER_LINEAR)
              .addInt("RL_TEXTURE_FILTER_MIP_NEAREST", RL_TEXTURE_FILTER_MIP_NEAREST)
              .addInt("RL_TEXTURE_FILTER_NEAREST_MIP_LINEAR", RL_TEXTURE_FILTER_NEAREST_MIP_LINEAR)
              .addInt("RL_TEXTURE_FILTER_LINEAR_MIP_NEAREST", RL_TEXTURE_FILTER_LINEAR_MIP_NEAREST)
              .addInt("RL_TEXTURE_FILTER_MIP_LINEAR", RL_TEXTURE_FILTER_MIP_LINEAR)
              .addInt("RL_TEXTURE_FILTER_ANISOTROPIC", RL_TEXTURE_FILTER_ANISOTROPIC)
              .addInt("RL_TEXTURE_WRAP_REPEAT", RL_TEXTURE_WRAP_REPEAT)
              .addInt("RL_TEXTURE_WRAP_CLAMP", RL_TEXTURE_WRAP_CLAMP)
              .addInt("RL_TEXTURE_WRAP_MIRROR_REPEAT", RL_TEXTURE_WRAP_MIRROR_REPEAT)
              .addInt("RL_TEXTURE_WRAP_MIRROR_CLAMP", RL_TEXTURE_WRAP_MIRROR_CLAMP)

        // Blend Modes
              .addInt("RL_BLEND_ALPHA", RL_BLEND_ALPHA)
              .addInt("RL_BLEND_ADDITIVE", RL_BLEND_ADDITIVE)
              .addInt("RL_BLEND_MULTIPLIED", RL_BLEND_MULTIPLIED)
              .addInt("RL_BLEND_ADD_COLORS", RL_BLEND_ADD_COLORS)
              .addInt("RL_BLEND_SUBTRACT_COLORS", RL_BLEND_SUBTRACT_COLORS)
              .addInt("RL_BLEND_ALPHA_PREMULTIPLY", RL_BLEND_ALPHA_PREMULTIPLY)
              .addInt("RL_BLEND_CUSTOM", RL_BLEND_CUSTOM)
              .addInt("RL_BLEND_CUSTOM_SEPARATE", RL_BLEND_CUSTOM_SEPARATE)

        // Attachments
              .addInt("RL_ATTACHMENT_COLOR_CHANNEL0", RL_ATTACHMENT_COLOR_CHANNEL0)
              .addInt("RL_ATTACHMENT_COLOR_CHANNEL1", RL_ATTACHMENT_COLOR_CHANNEL1)
              .addInt("RL_ATTACHMENT_COLOR_CHANNEL2", RL_ATTACHMENT_COLOR_CHANNEL2)
              .addInt("RL_ATTACHMENT_DEPTH", RL_ATTACHMENT_DEPTH)
              .addInt("RL_ATTACHMENT_STENCIL", RL_ATTACHMENT_STENCIL)
              .addInt("RL_ATTACHMENT_TEXTURE2D", RL_ATTACHMENT_TEXTURE2D)
              .addInt("RL_ATTACHMENT_RENDERBUFFER", RL_ATTACHMENT_RENDERBUFFER)
              .addInt("RL_ATTACHMENT_CUBEMAP_POSITIVE_X", RL_ATTACHMENT_CUBEMAP_POSITIVE_X)
              .addInt("RL_ATTACHMENT_CUBEMAP_NEGATIVE_X", RL_ATTACHMENT_CUBEMAP_NEGATIVE_X)
              .addInt("RL_ATTACHMENT_CUBEMAP_POSITIVE_Y", RL_ATTACHMENT_CUBEMAP_POSITIVE_Y)
              .addInt("RL_ATTACHMENT_CUBEMAP_NEGATIVE_Y", RL_ATTACHMENT_CUBEMAP_NEGATIVE_Y)
              .addInt("RL_ATTACHMENT_CUBEMAP_POSITIVE_Z", RL_ATTACHMENT_CUBEMAP_POSITIVE_Z)
              .addInt("RL_ATTACHMENT_CUBEMAP_NEGATIVE_Z", RL_ATTACHMENT_CUBEMAP_NEGATIVE_Z)

        // Shader Types
              .addInt("RL_FRAGMENT_SHADER", RL_FRAGMENT_SHADER)
              .addInt("RL_VERTEX_SHADER", RL_VERTEX_SHADER)
              .addInt("RL_COMPUTE_SHADER", RL_COMPUTE_SHADER)

        // Uniform Types
              .addInt("RL_SHADER_UNIFORM_FLOAT", RL_SHADER_UNIFORM_FLOAT)
              .addInt("RL_SHADER_UNIFORM_VEC2", RL_SHADER_UNIFORM_VEC2)
              .addInt("RL_SHADER_UNIFORM_VEC3", RL_SHADER_UNIFORM_VEC3)
              .addInt("RL_SHADER_UNIFORM_VEC4", RL_SHADER_UNIFORM_VEC4)
              .addInt("RL_SHADER_UNIFORM_INT", RL_SHADER_UNIFORM_INT)
              .addInt("RL_SHADER_UNIFORM_IVEC2", RL_SHADER_UNIFORM_IVEC2)
              .addInt("RL_SHADER_UNIFORM_IVEC3", RL_SHADER_UNIFORM_IVEC3)
              .addInt("RL_SHADER_UNIFORM_IVEC4", RL_SHADER_UNIFORM_IVEC4)
              .addInt("RL_SHADER_UNIFORM_SAMPLER2D", RL_SHADER_UNIFORM_SAMPLER2D)

        // Pixel formats (commonly used)
              .addInt("RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE", RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE)
              .addInt("RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8", RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8)
              .addInt("RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8", RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
              .addInt("RL_PIXELFORMAT_UNCOMPRESSED_R32", RL_PIXELFORMAT_UNCOMPRESSED_R32)
              .addInt("RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32", RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32)
              .addInt("RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32", RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32)
              .addInt("RL_PIXELFORMAT_UNCOMPRESSED_R16", RL_PIXELFORMAT_UNCOMPRESSED_R16)
              .addInt("RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16", RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16)
              .addInt("RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16", RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
    }

    void registerAll(Interpreter &vm)
    {
        register_rlgl(vm);
    }
}
