#include "raylib.h"
#include "jolt_core.hpp"

#ifdef JPH_DEBUG_RENDERER

#include <Jolt/Renderer/DebugRendererSimple.h>
#include <Jolt/Physics/Body/BodyManager.h>

namespace JoltBindings
{
    using namespace JPH;

    // ─── Raylib-backed Jolt DebugRendererSimple ──────────────────────────

    class RaylibDebugRenderer final : public DebugRendererSimple
    {
    public:
        float fillAlpha = 0.4f;

        RaylibDebugRenderer() : DebugRendererSimple() {}

        void DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor) override
        {
            Vector3 a = {(float)inFrom.GetX(), (float)inFrom.GetY(), (float)inFrom.GetZ()};
            Vector3 b = {(float)inTo.GetX(), (float)inTo.GetY(), (float)inTo.GetZ()};
            ::Color c = {inColor.r, inColor.g, inColor.b, inColor.a};
            ::DrawLine3D(a, b, c);
        }

        void DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow /*inCastShadow*/) override
        {
            Vector3 v1 = {(float)inV1.GetX(), (float)inV1.GetY(), (float)inV1.GetZ()};
            Vector3 v2 = {(float)inV2.GetX(), (float)inV2.GetY(), (float)inV2.GetZ()};
            Vector3 v3 = {(float)inV3.GetX(), (float)inV3.GetY(), (float)inV3.GetZ()};
            ::Color c = {inColor.r, inColor.g, inColor.b, (unsigned char)(inColor.a * fillAlpha)};
            ::DrawTriangle3D(v1, v2, v3, c);
            // Also draw backface
            ::DrawTriangle3D(v3, v2, v1, c);
        }

        void DrawText3D(RVec3Arg /*inPosition*/, const string_view &/*inString*/, ColorArg /*inColor*/, float /*inHeight*/) override
        {
            // 3D text rendering not supported via raylib, skip silently
        }
    };

    static RaylibDebugRenderer *g_debugRenderer = nullptr;

    static RaylibDebugRenderer *getDebugRenderer()
    {
        if (!g_debugRenderer)
            g_debugRenderer = new RaylibDebugRenderer();
        return g_debugRenderer;
    }

    // ─── JoltWorld.debugDraw([options]) ──────────────────────────────────
    // Options via bit flags (default = shapes wireframe):
    //   drawShapes          (default true)
    //   drawWireframe       (default true)
    //   drawBoundingBoxes   (default false)
    //   drawConstraints     (default true)
    //   drawConstraintLimits(default false)
    //   drawCenterOfMass    (default false)
    //   drawVelocity        (default false)

    static int jolt_world_debug_draw(Interpreter *vm, void *instance, int argCount, Value *args)
    {
        if (argCount > 1)
        {
            Error("JoltWorld.debugDraw() expects 0 or 1 arguments (flags)");
            return 0;
        }

        JoltWorldHandle *world = require_world(instance, "JoltWorld.debugDraw()");
        if (!world)
            return 0;

        // Default flags: shapes + wireframe + constraints
        int flags = 0x01 | 0x02 | 0x08;
        if (argCount == 1 && args[0].isNumber())
            flags = args[0].asInt();

        RaylibDebugRenderer *renderer = getDebugRenderer();

        // Set camera pos for LOD (use raylib's current 3D camera)
        // We can't easily obtain it here, so skip LOD — DebugRendererSimple handles it

        BodyManager::DrawSettings settings;
        settings.mDrawShape                 = (flags & 0x01) != 0;
        settings.mDrawShapeWireframe        = (flags & 0x02) != 0;
        settings.mDrawBoundingBox           = (flags & 0x04) != 0;
        settings.mDrawCenterOfMassTransform = (flags & 0x10) != 0;
        settings.mDrawVelocity              = (flags & 0x20) != 0;
        settings.mDrawMassAndInertia        = (flags & 0x40) != 0;
        settings.mDrawSleepStats            = (flags & 0x80) != 0;

        world->physicsSystem.DrawBodies(settings, renderer);

        if ((flags & 0x08) != 0)
            world->physicsSystem.DrawConstraints(renderer);

        if ((flags & 0x100) != 0)
            world->physicsSystem.DrawConstraintLimits(renderer);

        if ((flags & 0x200) != 0)
            world->physicsSystem.DrawConstraintReferenceFrame(renderer);

        renderer->NextFrame();

        return 0;
    }

    // ─── JoltWorld.setDebugDrawAlpha(alpha) ──────────────────────────────

    static int jolt_world_set_debug_alpha(Interpreter *vm, void *instance, int argCount, Value *args)
    {
        (void)instance;
        if (argCount != 1 || !args[0].isNumber())
        {
            Error("JoltWorld.setDebugDrawAlpha() expects (float alpha)");
            return 0;
        }

        float alpha = (float)args[0].asNumber();
        if (alpha < 0.0f) alpha = 0.0f;
        if (alpha > 1.0f) alpha = 1.0f;
        getDebugRenderer()->fillAlpha = alpha;
        return 0;
    }

    // ─── Registration ────────────────────────────────────────────────────

    void register_jolt_debug(Interpreter &vm)
    {
        vm.addNativeMethod(g_worldClass, "debugDraw", jolt_world_debug_draw);
        vm.addNativeMethod(g_worldClass, "setDebugDrawAlpha", jolt_world_set_debug_alpha);

        // Flag constants for debugDraw
        ModuleBuilder module = vm.addModule("JoltDebug");
        module.addInt("JOLT_DRAW_SHAPES",              0x01)
              .addInt("JOLT_DRAW_WIREFRAME",           0x02)
              .addInt("JOLT_DRAW_BOUNDING_BOXES",      0x04)
              .addInt("JOLT_DRAW_CONSTRAINTS",         0x08)
              .addInt("JOLT_DRAW_CENTER_OF_MASS",      0x10)
              .addInt("JOLT_DRAW_VELOCITY",            0x20)
              .addInt("JOLT_DRAW_MASS_AND_INERTIA",    0x40)
              .addInt("JOLT_DRAW_SLEEP_STATS",         0x80)
              .addInt("JOLT_DRAW_CONSTRAINT_LIMITS",   0x100)
              .addInt("JOLT_DRAW_CONSTRAINT_FRAMES",   0x200)
              .addInt("JOLT_DRAW_DEFAULT",             0x01 | 0x02 | 0x08);
    }

    void cleanup_jolt_debug()
    {
        delete g_debugRenderer;
        g_debugRenderer = nullptr;
    }
}

#endif // JPH_DEBUG_RENDERER
