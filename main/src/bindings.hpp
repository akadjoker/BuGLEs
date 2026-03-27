#pragma once

#include "interpreter.hpp"

namespace Bindings
{
    void set_gl_debug_enabled(bool enabled);
    bool get_gl_debug_enabled();
    bool check_gl_errors(Interpreter *vm, const char *fnName);

    void registerAll(Interpreter &vm);
    void register_core(ModuleBuilder &mod);

    void register_device_input(Interpreter &vm);
    void register_opengl(ModuleBuilder &module);
    void register_opengl_vertexbuffer(ModuleBuilder &module);
    void register_opengl_texture(ModuleBuilder &module);
    void register_opengl_rendertarget(ModuleBuilder &module);
    void register_opengl_shader(ModuleBuilder &module);
    void register_opengl_instancing(ModuleBuilder &module);
    void register_opengl_query(ModuleBuilder &module);
#if !defined(GRAPHICS_API_OPENGL_ES2) && !defined(GRAPHICS_API_OPENGL_ES3)
    void register_opengl_ubo(ModuleBuilder &module);
    void register_opengl_compute(ModuleBuilder &module);
    void register_opengl_transform_feedback(ModuleBuilder &module);
    void register_opengl_advanced(ModuleBuilder &module);
#endif
    void register_stb(ModuleBuilder &module);
    void register_stb_rect_pack(ModuleBuilder &module, Interpreter &vm);
    void register_stb_truetype(ModuleBuilder &module, Interpreter &vm);
    void register_msf_gif(Interpreter &vm);
    void register_poly2tri(Interpreter &vm);
    void register_raymath(Interpreter &vm);
    void register_audio(ModuleBuilder &module);
    void register_shapes(ModuleBuilder &module);
}

namespace ImGuiBindings
{
    void registerAll(Interpreter &vm);
}

namespace RaylibBindings
{
    void register_structs(Interpreter &vm);
    void register_core(Interpreter &vm);
    void register_camera(Interpreter &vm);
    void register_models(Interpreter &vm);
    void register_shapes(Interpreter &vm);
    void register_text(Interpreter &vm);
    void register_textures(Interpreter &vm);
}

namespace RLGLBindings
{
    void register_rlgl(Interpreter &vm);
    void registerAll(Interpreter &vm);
}

 
void registerBuiltinModules(Interpreter &vm);

