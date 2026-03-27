#include "bindings.hpp"
#include "raylib.h"

namespace RaylibBindings
{
    static NativeClassDef *g_camera3DClass = nullptr;

    static Camera3D *require_camera(void *instance, const char *fn)
    {
        Camera3D *cam = (Camera3D *)instance;
        if (!cam) { Error("%s on invalid Camera3D", fn); return nullptr; }
        return cam;
    }

    // Helper: extract Camera3D* from either NativeClass instance or raw pointer (backwards compat)
    static Camera3D *extract_camera(const Value &v)
    {
        if (v.isNativeClassInstance())
        {
            NativeClassInstance *inst = v.asNativeClassInstance();
            if (inst && inst->userData) return (Camera3D *)inst->userData;
        }
        if (v.isPointer()) return (Camera3D *)v.asPointer();
        return nullptr;
    }

    // ========================================
    // CAMERA 2D
    // ========================================

    static int native_BeginMode2D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNativeStructInstance())
        {
            Error("BeginMode2D expects Camera2D");
            return 0;
        }
        auto *inst = args[0].asNativeStructInstance();
        Camera2D *camera = (Camera2D *)inst->data;
        BeginMode2D(*camera);
        return 0;
    }

    static int native_EndMode2D(Interpreter *vm, int argc, Value *args)
    {
        EndMode2D();
        return 0;
    }

    static int native_GetScreenToWorld2D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("GetScreenToWorld2D expects 2 arguments");
            return 0;
        }
        if (!args[0].isNativeStructInstance() || !args[1].isNativeStructInstance())
        {
            Error("GetScreenToWorld2D expects Vector2 and Camera2D");
            return 0;
        }
        auto *posInst = args[0].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;
        auto *camInst = args[1].asNativeStructInstance();
        Camera2D *camera = (Camera2D *)camInst->data;

        Vector2 result = GetScreenToWorld2D(*pos, *camera);
        vm->pushFloat(result.x);
        vm->pushFloat(result.y);
        return 2;
    }

    static int native_GetWorldToScreen2D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("GetWorldToScreen2D expects 2 arguments");
            return 0;
        }
        if (!args[0].isNativeStructInstance() || !args[1].isNativeStructInstance())
        {
            Error("GetWorldToScreen2D expects Vector2 and Camera2D");
            return 0;
        }
        auto *posInst = args[0].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;
        auto *camInst = args[1].asNativeStructInstance();
        Camera2D *camera = (Camera2D *)camInst->data;

        Vector2 result = GetWorldToScreen2D(*pos, *camera);
        vm->pushFloat(result.x);
        vm->pushFloat(result.y);
        return 2;
    }

    // ========================================
    // CAMERA 3D (NativeClass with GC)
    // ========================================

    static int native_BeginMode3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("BeginMode3D expects Camera3D");
            return 0;
        }
        Camera3D *camera = extract_camera(args[0]);
        if (!camera) { Error("BeginMode3D expects Camera3D"); return 0; }
        BeginMode3D(*camera);
        return 0;
    }

    static int native_EndMode3D(Interpreter *vm, int argc, Value *args)
    {
        EndMode3D();
        return 0;
    }

    // ─── Constructor / Destructor ───────────────────────────────────────

    static void *camera3d_ctor(Interpreter *vm, int argCount, Value *args)
    {
        Camera3D *cam = new Camera3D();
        cam->position = {0.0f, 10.0f, 10.0f};
        cam->target   = {0.0f, 0.0f, 0.0f};
        cam->up       = {0.0f, 1.0f, 0.0f};
        cam->fovy     = 45.0f;
        cam->projection = CAMERA_PERSPECTIVE;
        return cam;
    }

    static void camera3d_dtor(Interpreter *vm, void *instance)
    {
        delete (Camera3D *)instance;
    }

    // ─── Methods ────────────────────────────────────────────────────────

    static int cam3d_set_position(Interpreter *vm, void *instance, int argc, Value *args)
    {
        if (argc != 3) { Error("Camera3D.setPosition(x,y,z)"); return 0; }
        Camera3D *cam = require_camera(instance, "setPosition");
        if (!cam) return 0;
        cam->position.x = (float)args[0].asNumber();
        cam->position.y = (float)args[1].asNumber();
        cam->position.z = (float)args[2].asNumber();
        return 0;
    }

    static int cam3d_get_position(Interpreter *vm, void *instance, int argc, Value *args)
    {
        Camera3D *cam = require_camera(instance, "getPosition");
        if (!cam) { vm->pushNil(); return 1; }
        vm->pushFloat(cam->position.x);
        vm->pushFloat(cam->position.y);
        vm->pushFloat(cam->position.z);
        return 3;
    }

    static int cam3d_set_target(Interpreter *vm, void *instance, int argc, Value *args)
    {
        if (argc != 3) { Error("Camera3D.setTarget(x,y,z)"); return 0; }
        Camera3D *cam = require_camera(instance, "setTarget");
        if (!cam) return 0;
        cam->target.x = (float)args[0].asNumber();
        cam->target.y = (float)args[1].asNumber();
        cam->target.z = (float)args[2].asNumber();
        return 0;
    }

    static int cam3d_get_target(Interpreter *vm, void *instance, int argc, Value *args)
    {
        Camera3D *cam = require_camera(instance, "getTarget");
        if (!cam) { vm->pushNil(); return 1; }
        vm->pushFloat(cam->target.x);
        vm->pushFloat(cam->target.y);
        vm->pushFloat(cam->target.z);
        return 3;
    }

    static int cam3d_set_up(Interpreter *vm, void *instance, int argc, Value *args)
    {
        if (argc != 3) { Error("Camera3D.setUp(x,y,z)"); return 0; }
        Camera3D *cam = require_camera(instance, "setUp");
        if (!cam) return 0;
        cam->up.x = (float)args[0].asNumber();
        cam->up.y = (float)args[1].asNumber();
        cam->up.z = (float)args[2].asNumber();
        return 0;
    }

    static int cam3d_set_fovy(Interpreter *vm, void *instance, int argc, Value *args)
    {
        if (argc != 1) { Error("Camera3D.setFovy(fovy)"); return 0; }
        Camera3D *cam = require_camera(instance, "setFovy");
        if (!cam) return 0;
        cam->fovy = (float)args[0].asNumber();
        return 0;
    }

    static int cam3d_set_projection(Interpreter *vm, void *instance, int argc, Value *args)
    {
        if (argc != 1) { Error("Camera3D.setProjection(proj)"); return 0; }
        Camera3D *cam = require_camera(instance, "setProjection");
        if (!cam) return 0;
        cam->projection = (int)args[0].asNumber();
        return 0;
    }

    static int cam3d_update(Interpreter *vm, void *instance, int argc, Value *args)
    {
        if (argc != 1) { Error("Camera3D.update(mode)"); return 0; }
        Camera3D *cam = require_camera(instance, "update");
        if (!cam) return 0;
        UpdateCamera(cam, (int)args[0].asNumber());
        return 0;
    }

    static int cam3d_update_pro(Interpreter *vm, void *instance, int argc, Value *args)
    {
        if (argc != 7) { Error("Camera3D.updatePro(movX,movY,movZ,rotX,rotY,rotZ,zoom)"); return 0; }
        Camera3D *cam = require_camera(instance, "updatePro");
        if (!cam) return 0;
        Vector3 movement = {(float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()};
        Vector3 rotation = {(float)args[3].asNumber(), (float)args[4].asNumber(), (float)args[5].asNumber()};
        float zoom = (float)args[6].asNumber();
        UpdateCameraPro(cam, movement, rotation, zoom);
        return 0;
    }

    static int cam3d_get_world_to_screen(Interpreter *vm, void *instance, int argc, Value *args)
    {
        if (argc != 3) { Error("Camera3D.getWorldToScreen(x,y,z)"); vm->pushNil(); return 1; }
        Camera3D *cam = require_camera(instance, "getWorldToScreen");
        if (!cam) { vm->pushNil(); return 1; }
        Vector3 pos = {(float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()};
        Vector2 result = GetWorldToScreen(pos, *cam);
        vm->pushFloat(result.x);
        vm->pushFloat(result.y);
        return 2;
    }

    // ─── Backwards-compatible global wrappers ───────────────────────────
    // These accept both NativeClass and raw pointer for backwards compat

    static int native_CreateCamera3D(Interpreter *vm, int argc, Value *args)
    {
        // Legacy: return a raw pointer (kept for old scripts)
        Camera3D *camera = new Camera3D();
        camera->position = {0.0f, 10.0f, 10.0f};
        camera->target = {0.0f, 0.0f, 0.0f};
        camera->up = {0.0f, 1.0f, 0.0f};
        camera->fovy = 45.0f;
        camera->projection = CAMERA_PERSPECTIVE;
        vm->push(vm->makePointer(camera));
        return 1;
    }

    static int native_DestroyCamera3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        // Only delete raw pointers; NativeClass instances are GC'd
        if (args[0].isPointer())
        {
            Camera3D *camera = (Camera3D *)args[0].asPointer();
            delete camera;
        }
        return 0;
    }

    static int native_SetCamera3DPosition(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) { Error("SetCamera3DPosition expects Camera3D and x, y, z"); return 0; }
        Camera3D *camera = extract_camera(args[0]);
        if (!camera) { Error("SetCamera3DPosition expects Camera3D"); return 0; }
        camera->position.x = (float)args[1].asDouble();
        camera->position.y = (float)args[2].asDouble();
        camera->position.z = (float)args[3].asDouble();
        return 0;
    }

    static int native_SetCamera3DTarget(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) { Error("SetCamera3DTarget expects Camera3D and x, y, z"); return 0; }
        Camera3D *camera = extract_camera(args[0]);
        if (!camera) { Error("SetCamera3DTarget expects Camera3D"); return 0; }
        camera->target.x = (float)args[1].asDouble();
        camera->target.y = (float)args[2].asDouble();
        camera->target.z = (float)args[3].asDouble();
        return 0;
    }

    static int native_SetCamera3DUp(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) { Error("SetCamera3DUp expects Camera3D and x, y, z"); return 0; }
        Camera3D *camera = extract_camera(args[0]);
        if (!camera) { Error("SetCamera3DUp expects Camera3D"); return 0; }
        camera->up.x = (float)args[1].asDouble();
        camera->up.y = (float)args[2].asDouble();
        camera->up.z = (float)args[3].asDouble();
        return 0;
    }

    static int native_SetCamera3DFovy(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) { Error("SetCamera3DFovy expects Camera3D and fovy"); return 0; }
        Camera3D *camera = extract_camera(args[0]);
        if (!camera) { Error("SetCamera3DFovy expects Camera3D"); return 0; }
        camera->fovy = (float)args[1].asDouble();
        return 0;
    }

    static int native_SetCamera3DProjection(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) { Error("SetCamera3DProjection expects Camera3D and projection"); return 0; }
        Camera3D *camera = extract_camera(args[0]);
        if (!camera) { Error("SetCamera3DProjection expects Camera3D"); return 0; }
        camera->projection = (int)args[1].asNumber();
        return 0;
    }

    static int native_UpdateCamera(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) { Error("UpdateCamera expects Camera3D and mode"); return 0; }
        Camera3D *camera = extract_camera(args[0]);
        if (!camera) { Error("UpdateCamera expects Camera3D"); return 0; }
        UpdateCamera(camera, (int)args[1].asNumber());
        return 0;
    }

    static int native_GetWorldToScreen(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) { Error("GetWorldToScreen expects (x, y, z, Camera3D)"); return 0; }
        Camera3D *camera = extract_camera(args[3]);
        if (!camera) { Error("GetWorldToScreen expects Camera3D as arg 4"); return 0; }
        Vector3 pos = {(float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()};
        Vector2 result = GetWorldToScreen(pos, *camera);
        vm->pushFloat(result.x);
        vm->pushFloat(result.y);
        return 2;
    }

    static int native_UpdateCameraPro(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 8) { Error("UpdateCameraPro expects (Camera3D, movX, movY, movZ, rotX, rotY, rotZ, zoom)"); return 0; }
        Camera3D *camera = extract_camera(args[0]);
        if (!camera) { Error("UpdateCameraPro expects Camera3D"); return 0; }
        Vector3 movement = {(float)args[1].asNumber(), (float)args[2].asNumber(), (float)args[3].asNumber()};
        Vector3 rotation = {(float)args[4].asNumber(), (float)args[5].asNumber(), (float)args[6].asNumber()};
        float zoom = (float)args[7].asNumber();
        UpdateCameraPro(camera, movement, rotation, zoom);
        return 0;
    }

    // ========================================
    // REGISTER CAMERA
    // ========================================

    void register_camera(Interpreter &vm)
    {
        // Camera 2D
        vm.registerNative("BeginMode2D", native_BeginMode2D, 1);
        vm.registerNative("EndMode2D", native_EndMode2D, 0);
        vm.registerNative("GetScreenToWorld2D", native_GetScreenToWorld2D, 2);
        vm.registerNative("GetWorldToScreen2D", native_GetWorldToScreen2D, 2);

        // Camera 3D — NativeClass (GC-managed)
        g_camera3DClass = vm.registerNativeClass("Camera3D", camera3d_ctor, camera3d_dtor, 0, false);
        vm.addNativeMethod(g_camera3DClass, "setPosition", cam3d_set_position);
        vm.addNativeMethod(g_camera3DClass, "getPosition", cam3d_get_position);
        vm.addNativeMethod(g_camera3DClass, "setTarget", cam3d_set_target);
        vm.addNativeMethod(g_camera3DClass, "getTarget", cam3d_get_target);
        vm.addNativeMethod(g_camera3DClass, "setUp", cam3d_set_up);
        vm.addNativeMethod(g_camera3DClass, "setFovy", cam3d_set_fovy);
        vm.addNativeMethod(g_camera3DClass, "setProjection", cam3d_set_projection);
        vm.addNativeMethod(g_camera3DClass, "update", cam3d_update);
        vm.addNativeMethod(g_camera3DClass, "updatePro", cam3d_update_pro);
        vm.addNativeMethod(g_camera3DClass, "getWorldToScreen", cam3d_get_world_to_screen);

        // Camera 3D — global functions (BeginMode3D/EndMode3D + backwards-compat)
        vm.registerNative("BeginMode3D", native_BeginMode3D, 1);
        vm.registerNative("EndMode3D", native_EndMode3D, 0);
        vm.registerNative("CreateCamera3D", native_CreateCamera3D, 0);
        vm.registerNative("DestroyCamera3D", native_DestroyCamera3D, 1);
        vm.registerNative("SetCamera3DPosition", native_SetCamera3DPosition, 4);
        vm.registerNative("SetCamera3DTarget", native_SetCamera3DTarget, 4);
        vm.registerNative("SetCamera3DUp", native_SetCamera3DUp, 4);
        vm.registerNative("SetCamera3DFovy", native_SetCamera3DFovy, 2);
        vm.registerNative("SetCamera3DProjection", native_SetCamera3DProjection, 2);
        vm.registerNative("UpdateCamera", native_UpdateCamera, 2);
        vm.registerNative("GetWorldToScreen", native_GetWorldToScreen, 4);
        vm.registerNative("UpdateCameraPro", native_UpdateCameraPro, 8);

        // Camera mode constants
        vm.addGlobal("CAMERA_CUSTOM", vm.makeInt(CAMERA_CUSTOM));
        vm.addGlobal("CAMERA_FREE", vm.makeInt(CAMERA_FREE));
        vm.addGlobal("CAMERA_ORBITAL", vm.makeInt(CAMERA_ORBITAL));
        vm.addGlobal("CAMERA_FIRST_PERSON", vm.makeInt(CAMERA_FIRST_PERSON));
        vm.addGlobal("CAMERA_THIRD_PERSON", vm.makeInt(CAMERA_THIRD_PERSON));

        // Projection constants
        vm.addGlobal("CAMERA_PERSPECTIVE", vm.makeInt(CAMERA_PERSPECTIVE));
        vm.addGlobal("CAMERA_ORTHOGRAPHIC", vm.makeInt(CAMERA_ORTHOGRAPHIC));
    }

}
