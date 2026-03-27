#include "bindings.hpp"

#ifdef BUGL_WITH_BOX2D
#include "box2d/bindings.hpp"
#endif
#ifdef BUGL_WITH_JOLT
#include "jolt/bindings.hpp"
#endif
#ifdef BUGL_WITH_ASSIMP
#include "assimp/assimp_bindings.hpp"
#endif
#ifdef BUGL_WITH_MESHOPT
#include "meshopt/bindings.hpp"
#endif
#ifdef BUGL_WITH_MICROPATHER
#include "micropather/bindings.hpp"
#endif
#ifdef BUGL_WITH_OPENSTEER
#include "opensteer/bindings.hpp"
#endif
#ifdef BUGL_WITH_RECAST
#include "recast/recast_bindings.hpp"
#endif

namespace Bindings
{

    //

    void registerAll(Interpreter &vm)
    {
        register_device_input(vm);

        ModuleBuilder module = vm.addModule("Engine");
        register_core(module);
        
        module = vm.addModule("OpenGl");
        register_opengl(module);
        register_opengl_shader(module);
        register_opengl_instancing(module);
        register_opengl_query(module);
        register_opengl_vertexbuffer(module);
        register_opengl_texture(module);
        register_opengl_rendertarget(module);
#if defined(GRAPHICS_API_OPENGL_11)
        register_opengl_legacy(module);
#endif
#if !defined(GRAPHICS_API_OPENGL_ES2) && !defined(GRAPHICS_API_OPENGL_ES3)
        register_opengl_ubo(module);
        register_opengl_compute(module);
        register_opengl_transform_feedback(module);
        register_opengl_advanced(module);
#endif

        module = vm.addModule("STB");
        register_stb(module);
        register_stb_rect_pack(module, vm);
        register_stb_truetype(module, vm);
        register_msf_gif(vm);
        register_poly2tri(vm);
        register_raymath(vm);

        module = vm.addModule("Audio");
        register_audio(module);

        module = vm.addModule("Shapes");
        register_shapes(module);

        // Raylib bindings (all global, no module)
        RaylibBindings::register_structs(vm);
        RaylibBindings::register_core(vm);
        RaylibBindings::register_camera(vm);
        RaylibBindings::register_models(vm);
        RaylibBindings::register_shapes(vm);
        RaylibBindings::register_text(vm);
        RaylibBindings::register_textures(vm);

        ImGuiBindings::registerAll(vm);
    }
}

void registerBuiltinModules(Interpreter &vm)
{
    RLGLBindings::registerAll(vm);

#ifdef BUGL_WITH_BOX2D
    BOX2DBindings::register_box2d(vm);
    BOX2DBindings::register_box2d_joints(vm);
#endif
#ifdef BUGL_WITH_JOLT
    JoltBindings::registerAll(vm);
#endif
#ifdef BUGL_WITH_ASSIMP
    AssimpBindings::registerAll(vm);
#endif
#ifdef BUGL_WITH_MESHOPT
    MeshOptBindings::registerAll(vm);
#endif
#ifdef BUGL_WITH_MICROPATHER
    MicroPatherBindings::registerAll(vm);
#endif
#ifdef BUGL_WITH_OPENSTEER
    OpenSteerBindings::registerAll(vm);
#endif
#ifdef BUGL_WITH_RECAST
    RecastBindings::registerAll(vm);
#endif
}
