#include "bindings.hpp"
#include "raylib.h"

namespace RaylibBindings
{

    // ========================================
    // BASIC 3D SHAPES
    // ========================================

    static int native_DrawCube(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawCube expects 7 arguments (x, y, z, width, height, length, color)");
            return 0;
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawCube expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float width = args[3].asNumber();
        float height = args[4].asNumber();
        float length = args[5].asNumber();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCube({x, y, z}, width, height, length, *color);
        return 0;
    }

    static int native_DrawCubeWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawCubeWires expects 7 arguments");
            return 0;
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawCubeWires expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float width = args[3].asNumber();
        float height = args[4].asNumber();
        float length = args[5].asNumber();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCubeWires({x, y, z}, width, height, length, *color);
        return 0;
    }

    static int native_DrawSphere(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawSphere expects 5 arguments (x, y, z, radius, color)");
            return 0;
        }
        if (!args[4].isNativeStructInstance())
        {
            Error("DrawSphere expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float radius = args[3].asNumber();

        auto *colorInst = args[4].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawSphere({x, y, z}, radius, *color);
        return 0;
    }

    static int native_DrawSphereWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawSphereWires expects 7 arguments (x, y, z, radius, rings, slices, color)");
            return 0;
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawSphereWires expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float radius = args[3].asNumber();
        int rings = args[4].asNumber();
        int slices = args[5].asNumber();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawSphereWires({x, y, z}, radius, rings, slices, *color);
        return 0;
    }

    static int native_DrawCylinder(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 8)
        {
            Error("DrawCylinder expects 8 arguments (x, y, z, radiusTop, radiusBottom, height, slices, color)");
            return 0;
        }
        if (!args[7].isNativeStructInstance())
        {
            Error("DrawCylinder expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float radiusTop = args[3].asNumber();
        float radiusBottom = args[4].asNumber();
        float height = args[5].asNumber();
        int slices = args[6].asNumber();

        auto *colorInst = args[7].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCylinder({x, y, z}, radiusTop, radiusBottom, height, slices, *color);
        return 0;
    }

    static int native_DrawPlane(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawPlane expects 6 arguments (x, y, z, width, length, color)");
            return 0;
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawPlane expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float width = args[3].asNumber();
        float length = args[4].asNumber();

        auto *colorInst = args[5].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawPlane({x, y, z}, {width, length}, *color);
        return 0;
    }

    static int native_DrawGrid(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("DrawGrid expects 2 arguments (slices, spacing)");
            return 0;
        }
        int slices = args[0].asNumber();
        float spacing = args[1].asNumber();
        DrawGrid(slices, spacing);
        return 0;
    }

    static int native_DrawLine3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawLine3D expects 7 arguments (x1, y1, z1, x2, y2, z2, color)");
            return 0;
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawLine3D expects Color");
            return 0;
        }
        float x1 = args[0].asNumber();
        float y1 = args[1].asNumber();
        float z1 = args[2].asNumber();
        float x2 = args[3].asNumber();
        float y2 = args[4].asNumber();
        float z2 = args[5].asNumber();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawLine3D({x1, y1, z1}, {x2, y2, z2}, *color);
        return 0;
    }

    static int native_DrawPoint3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawPoint3D expects 4 arguments (x, y, z, color)");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawPoint3D expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();

        auto *colorInst = args[3].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawPoint3D({x, y, z}, *color);
        return 0;
    }

    // ========================================
    // MODEL LOADING
    // ========================================

    static int native_LoadModel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
        {
            Error("LoadModel expects filename");
            return 0;
        }
        const char *filename = args[0].asStringChars();
        Model model = LoadModel(filename);
        Model *ptr = new Model(model);
        vm->pushPointer(ptr);
        return 1;
    }

    static int native_UnloadModel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Model *model = (Model *)args[0].asPointer();
        UnloadModel(*model);
        delete model;
        return 0;
    }

    static int native_DrawModel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawModel expects 6 arguments (model, x, y, z, scale, tint)");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawModel expects Model");
            return 0;
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawModel expects Color tint");
            return 0;
        }

        Model *model = (Model *)args[0].asPointer();
        float x = args[1].asNumber();
        float y = args[2].asNumber();
        float z = args[3].asNumber();
        float scale = args[4].asNumber();

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawModel(*model, {x, y, z}, scale, *tint);
        return 0;
    }

    static int native_DrawModelWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawModelWires expects 6 arguments");
            return 0;
        }
        if (!args[0].isPointer() || !args[5].isNativeStructInstance())
        {
            Error("DrawModelWires expects Model and Color");
            return 0;
        }

        Model *model = (Model *)args[0].asPointer();
        float x = args[1].asNumber();
        float y = args[2].asNumber();
        float z = args[3].asNumber();
        float scale = args[4].asNumber();

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawModelWires(*model, {x, y, z}, scale, *tint);
        return 0;
    }

    // ========================================
    // MESH GENERATION
    // ========================================

    static int native_GenMeshCube(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("GenMeshCube expects 3 arguments (width, height, length)");
            return 0;
        }
        float width = args[0].asNumber();
        float height = args[1].asNumber();
        float length = args[2].asNumber();

        Mesh mesh = GenMeshCube(width, height, length);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_GenMeshSphere(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("GenMeshSphere expects 3 arguments (radius, rings, slices)");
            return 0;
        }
        float radius = args[0].asNumber();
        int rings = args[1].asNumber();
        int slices = args[2].asNumber();

        Mesh mesh = GenMeshSphere(radius, rings, slices);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_GenMeshPlane(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("GenMeshPlane expects 4 arguments (width, length, resX, resZ)");
            return 0;
        }
        float width = args[0].asNumber();
        float length = args[1].asNumber();
        int resX = args[2].asNumber();
        int resZ = args[3].asNumber();

        Mesh mesh = GenMeshPlane(width, length, resX, resZ);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_GenMeshCylinder(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("GenMeshCylinder expects 3 arguments (radius, height, slices)");
            return 0;
        }
        float radius = args[0].asNumber();
        float height = args[1].asNumber();
        int slices = args[2].asNumber();

        Mesh mesh = GenMeshCylinder(radius, height, slices);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    // ========================================
    // ADDITIONAL 3D SHAPES
    // ========================================

    static int native_DrawCircle3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6 || !args[5].isNativeStructInstance()) return 0;
        Vector3 center = {(float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()};
        float radius = args[3].asNumber();
        // rotationAxis = {0,1,0}, angle = args[4]
        Color *color = (Color *)args[5].asNativeStructInstance()->data;
        DrawCircle3D(center, radius, {0,1,0}, args[4].asNumber(), *color);
        return 0;
    }

    static int native_DrawTriangle3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 10 || !args[9].isNativeStructInstance()) return 0;
        Vector3 v1 = {(float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()};
        Vector3 v2 = {(float)args[3].asNumber(), (float)args[4].asNumber(), (float)args[5].asNumber()};
        Vector3 v3 = {(float)args[6].asNumber(), (float)args[7].asNumber(), (float)args[8].asNumber()};
        Color *color = (Color *)args[9].asNativeStructInstance()->data;
        DrawTriangle3D(v1, v2, v3, *color);
        return 0;
    }

    static int native_DrawCapsule(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 10 || !args[9].isNativeStructInstance()) return 0;
        Vector3 start = {(float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()};
        Vector3 end = {(float)args[3].asNumber(), (float)args[4].asNumber(), (float)args[5].asNumber()};
        float radius = args[6].asNumber();
        int slices = args[7].asNumber();
        int rings = args[8].asNumber();
        Color *color = (Color *)args[9].asNativeStructInstance()->data;
        DrawCapsule(start, end, radius, slices, rings, *color);
        return 0;
    }

    static int native_DrawCapsuleWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 10 || !args[9].isNativeStructInstance()) return 0;
        Vector3 start = {(float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()};
        Vector3 end = {(float)args[3].asNumber(), (float)args[4].asNumber(), (float)args[5].asNumber()};
        float radius = args[6].asNumber();
        int slices = args[7].asNumber();
        int rings = args[8].asNumber();
        Color *color = (Color *)args[9].asNativeStructInstance()->data;
        DrawCapsuleWires(start, end, radius, slices, rings, *color);
        return 0;
    }

    static int native_IsModelReady(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Model *model = (Model *)args[0].asPointer();
        vm->pushBool(IsModelReady(*model));
        return 1;
    }

    // ========================================
    // CYLINDER WIRES
    // ========================================

    static int native_DrawCylinderWires(Interpreter *vm, int argc, Value *args)
    {
        // DrawCylinderWires(x, y, z, radiusTop, radiusBottom, height, slices, color)
        if (argc != 8 || !args[7].isNativeStructInstance()) return 0;
        Vector3 pos = {(float)args[0].asNumber(), (float)args[1].asNumber(), (float)args[2].asNumber()};
        float radiusTop = args[3].asNumber();
        float radiusBottom = args[4].asNumber();
        float height = args[5].asNumber();
        int slices = args[6].asNumber();
        Color *color = (Color *)args[7].asNativeStructInstance()->data;
        DrawCylinderWires(pos, radiusTop, radiusBottom, height, slices, *color);
        return 0;
    }

    // ========================================
    // EXTRA MESH GEN
    // ========================================

    static int native_GenMeshCone(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        Mesh mesh = GenMeshCone(args[0].asNumber(), args[1].asNumber(), (int)args[2].asNumber());
        Model *model = new Model();
        *model = LoadModelFromMesh(mesh);
        vm->push(vm->makePointer(model));
        return 1;
    }

    static int native_GenMeshTorus(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) return 0;
        Mesh mesh = GenMeshTorus(args[0].asNumber(), args[1].asNumber(), (int)args[2].asNumber(), (int)args[3].asNumber());
        Model *model = new Model();
        *model = LoadModelFromMesh(mesh);
        vm->push(vm->makePointer(model));
        return 1;
    }

    static int native_GenMeshKnot(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) return 0;
        Mesh mesh = GenMeshKnot(args[0].asNumber(), args[1].asNumber(), (int)args[2].asNumber(), (int)args[3].asNumber());
        Model *model = new Model();
        *model = LoadModelFromMesh(mesh);
        vm->push(vm->makePointer(model));
        return 1;
    }

    // ========================================
    // MODEL EXTRAS
    // ========================================

    static int native_LoadModelFromMesh(Interpreter *vm, int argc, Value *args)
    {
        // We expect a mesh pointer - but GenMesh* already returns Model, so this is
        // for manually created meshes. For now just alias to LoadModel behavior.
        // Actually, LoadModelFromMesh takes a Mesh, not a filepath.
        // Since our GenMesh* functions already convert mesh -> model, this is less useful.
        // But we expose it as: LoadModelFromMesh(meshPointer)
        // Not easily exposed without raw Mesh handling. Skip for now.
        return 0;
    }

    static int native_SetMaterialTexture(Interpreter *vm, int argc, Value *args)
    {
        // SetMaterialTexture(model, mapType, texture)
        if (argc != 3 || !args[0].isPointer() || !args[2].isPointer()) return 0;
        Model *model = (Model *)args[0].asPointer();
        int mapType = args[1].asInt();
        Texture2D *tex = (Texture2D *)args[2].asPointer();
        if (model->materialCount > 0)
        {
            SetMaterialTexture(&model->materials[0], mapType, *tex);
        }
        return 0;
    }

    static int native_GetModelBoundingBox(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Model *model = (Model *)args[0].asPointer();
        BoundingBox box = GetModelBoundingBox(*model);
        // Return min.x, min.y, min.z, max.x, max.y, max.z
        vm->pushFloat(box.min.x);
        vm->pushFloat(box.min.y);
        vm->pushFloat(box.min.z);
        vm->pushFloat(box.max.x);
        vm->pushFloat(box.max.y);
        vm->pushFloat(box.max.z);
        return 6;
    }

    static int native_DrawBillboard(Interpreter *vm, int argc, Value *args)
    {
        // DrawBillboard(camera3d, texture, x, y, z, size, color)
        if (argc != 7 || !args[0].isPointer() || !args[1].isPointer() || !args[6].isNativeStructInstance()) return 0;
        Camera3D *camera = (Camera3D *)args[0].asPointer();
        Texture2D *tex = (Texture2D *)args[1].asPointer();
        Vector3 pos = {(float)args[2].asNumber(), (float)args[3].asNumber(), (float)args[4].asNumber()};
        float size = args[5].asNumber();
        Color *tint = (Color *)args[6].asNativeStructInstance()->data;
        DrawBillboard(*camera, *tex, pos, size, *tint);
        return 0;
    }

    // ========================================
    // 3D COLLISION DETECTION
    // ========================================

    static int native_CheckCollisionSpheres(Interpreter *vm, int argc, Value *args)
    {
        // CheckCollisionSpheres(center1: Vector3, radius1, center2: Vector3, radius2)
        if (argc != 4) return 0;
        Vector3 *c1 = (Vector3 *)args[0].asNativeStructInstance()->data;
        Vector3 *c2 = (Vector3 *)args[2].asNativeStructInstance()->data;
        vm->pushBool(CheckCollisionSpheres(*c1, (float)args[1].asNumber(), *c2, (float)args[3].asNumber()));
        return 1;
    }

    static int native_CheckCollisionBoxes(Interpreter *vm, int argc, Value *args)
    {
        // CheckCollisionBoxes(min1: Vector3, max1: Vector3, min2: Vector3, max2: Vector3)
        if (argc != 4) return 0;
        Vector3 *min1 = (Vector3 *)args[0].asNativeStructInstance()->data;
        Vector3 *max1 = (Vector3 *)args[1].asNativeStructInstance()->data;
        Vector3 *min2 = (Vector3 *)args[2].asNativeStructInstance()->data;
        Vector3 *max2 = (Vector3 *)args[3].asNativeStructInstance()->data;
        BoundingBox b1 = {*min1, *max1};
        BoundingBox b2 = {*min2, *max2};
        vm->pushBool(CheckCollisionBoxes(b1, b2));
        return 1;
    }

    static int native_CheckCollisionBoxSphere(Interpreter *vm, int argc, Value *args)
    {
        // CheckCollisionBoxSphere(boxMin: Vector3, boxMax: Vector3, center: Vector3, radius)
        if (argc != 4) return 0;
        Vector3 *bmin = (Vector3 *)args[0].asNativeStructInstance()->data;
        Vector3 *bmax = (Vector3 *)args[1].asNativeStructInstance()->data;
        Vector3 *center = (Vector3 *)args[2].asNativeStructInstance()->data;
        BoundingBox box = {*bmin, *bmax};
        vm->pushBool(CheckCollisionBoxSphere(box, *center, (float)args[3].asNumber()));
        return 1;
    }

    // RayCollision return: hit, distance, point.x, point.y, point.z, normal.x, normal.y, normal.z

    static int native_GetRayCollisionSphere(Interpreter *vm, int argc, Value *args)
    {
        // GetRayCollisionSphere(rayPos: Vector3, rayDir: Vector3, center: Vector3, radius)
        if (argc != 4) return 0;
        Vector3 *pos = (Vector3 *)args[0].asNativeStructInstance()->data;
        Vector3 *dir = (Vector3 *)args[1].asNativeStructInstance()->data;
        Vector3 *center = (Vector3 *)args[2].asNativeStructInstance()->data;
        Ray ray = {*pos, *dir};
        RayCollision rc = GetRayCollisionSphere(ray, *center, (float)args[3].asNumber());
        vm->pushBool(rc.hit);
        vm->pushFloat(rc.distance);
        vm->pushFloat(rc.point.x);
        vm->pushFloat(rc.point.y);
        vm->pushFloat(rc.point.z);
        vm->pushFloat(rc.normal.x);
        vm->pushFloat(rc.normal.y);
        vm->pushFloat(rc.normal.z);
        return 8;
    }

    static int native_GetRayCollisionBox(Interpreter *vm, int argc, Value *args)
    {
        // GetRayCollisionBox(rayPos: Vector3, rayDir: Vector3, boxMin: Vector3, boxMax: Vector3)
        if (argc != 4) return 0;
        Vector3 *pos = (Vector3 *)args[0].asNativeStructInstance()->data;
        Vector3 *dir = (Vector3 *)args[1].asNativeStructInstance()->data;
        Vector3 *bmin = (Vector3 *)args[2].asNativeStructInstance()->data;
        Vector3 *bmax = (Vector3 *)args[3].asNativeStructInstance()->data;
        Ray ray = {*pos, *dir};
        BoundingBox box = {*bmin, *bmax};
        RayCollision rc = GetRayCollisionBox(ray, box);
        vm->pushBool(rc.hit);
        vm->pushFloat(rc.distance);
        vm->pushFloat(rc.point.x);
        vm->pushFloat(rc.point.y);
        vm->pushFloat(rc.point.z);
        vm->pushFloat(rc.normal.x);
        vm->pushFloat(rc.normal.y);
        vm->pushFloat(rc.normal.z);
        return 8;
    }

    static int native_GetRayCollisionMesh(Interpreter *vm, int argc, Value *args)
    {
        // GetRayCollisionMesh(rayPos: Vector3, rayDir: Vector3, model, meshIndex, transform[16])
        if (argc != 5) return 0;
        Vector3 *pos = (Vector3 *)args[0].asNativeStructInstance()->data;
        Vector3 *dir = (Vector3 *)args[1].asNativeStructInstance()->data;
        if (!args[2].isPointer()) return 0;
        Model *model = (Model *)args[2].asPointer();
        int meshIdx = args[3].asInt();
        if (meshIdx < 0 || meshIdx >= model->meshCount) return 0;
        if (!args[4].isArray()) return 0;
        ArrayInstance *a = args[4].asArray();
        if ((int)a->values.size() < 16) return 0;
        float f[16];
        for (int i = 0; i < 16; i++) f[i] = (float)a->values[i].asNumber();
        Matrix transform = {f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],
                            f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]};
        Ray ray = {*pos, *dir};
        RayCollision rc = GetRayCollisionMesh(ray, model->meshes[meshIdx], transform);
        vm->pushBool(rc.hit);
        vm->pushFloat(rc.distance);
        vm->pushFloat(rc.point.x);
        vm->pushFloat(rc.point.y);
        vm->pushFloat(rc.point.z);
        vm->pushFloat(rc.normal.x);
        vm->pushFloat(rc.normal.y);
        vm->pushFloat(rc.normal.z);
        return 8;
    }

    static int native_GetRayCollisionTriangle(Interpreter *vm, int argc, Value *args)
    {
        // GetRayCollisionTriangle(rayPos: Vector3, rayDir: Vector3, p1: Vector3, p2: Vector3, p3: Vector3)
        if (argc != 5) return 0;
        Vector3 *pos = (Vector3 *)args[0].asNativeStructInstance()->data;
        Vector3 *dir = (Vector3 *)args[1].asNativeStructInstance()->data;
        Vector3 *p1 = (Vector3 *)args[2].asNativeStructInstance()->data;
        Vector3 *p2 = (Vector3 *)args[3].asNativeStructInstance()->data;
        Vector3 *p3 = (Vector3 *)args[4].asNativeStructInstance()->data;
        Ray ray = {*pos, *dir};
        RayCollision rc = GetRayCollisionTriangle(ray, *p1, *p2, *p3);
        vm->pushBool(rc.hit);
        vm->pushFloat(rc.distance);
        vm->pushFloat(rc.point.x);
        vm->pushFloat(rc.point.y);
        vm->pushFloat(rc.point.z);
        vm->pushFloat(rc.normal.x);
        vm->pushFloat(rc.normal.y);
        vm->pushFloat(rc.normal.z);
        return 8;
    }

    static int native_GetRayCollisionQuad(Interpreter *vm, int argc, Value *args)
    {
        // GetRayCollisionQuad(rayPos: Vector3, rayDir: Vector3, p1, p2, p3, p4: Vector3)
        if (argc != 6) return 0;
        Vector3 *pos = (Vector3 *)args[0].asNativeStructInstance()->data;
        Vector3 *dir = (Vector3 *)args[1].asNativeStructInstance()->data;
        Vector3 *p1 = (Vector3 *)args[2].asNativeStructInstance()->data;
        Vector3 *p2 = (Vector3 *)args[3].asNativeStructInstance()->data;
        Vector3 *p3 = (Vector3 *)args[4].asNativeStructInstance()->data;
        Vector3 *p4 = (Vector3 *)args[5].asNativeStructInstance()->data;
        Ray ray = {*pos, *dir};
        RayCollision rc = GetRayCollisionQuad(ray, *p1, *p2, *p3, *p4);
        vm->pushBool(rc.hit);
        vm->pushFloat(rc.distance);
        vm->pushFloat(rc.point.x);
        vm->pushFloat(rc.point.y);
        vm->pushFloat(rc.point.z);
        vm->pushFloat(rc.normal.x);
        vm->pushFloat(rc.normal.y);
        vm->pushFloat(rc.normal.z);
        return 8;
    }

    // ========================================
    // REGISTER MODELS
    // ========================================

    void register_models(Interpreter &vm)
    {
        // Basic 3D shapes
        vm.registerNative("DrawCube", native_DrawCube, 7);
        vm.registerNative("DrawCubeWires", native_DrawCubeWires, 7);
        vm.registerNative("DrawSphere", native_DrawSphere, 5);
        vm.registerNative("DrawSphereWires", native_DrawSphereWires, 7);
        vm.registerNative("DrawCylinder", native_DrawCylinder, 8);
        vm.registerNative("DrawCapsule", native_DrawCapsule, 10);
        vm.registerNative("DrawCapsuleWires", native_DrawCapsuleWires, 10);
        vm.registerNative("DrawPlane", native_DrawPlane, 6);
        vm.registerNative("DrawGrid", native_DrawGrid, 2);
        vm.registerNative("DrawLine3D", native_DrawLine3D, 7);
        vm.registerNative("DrawPoint3D", native_DrawPoint3D, 4);
        vm.registerNative("DrawCircle3D", native_DrawCircle3D, 6);
        vm.registerNative("DrawTriangle3D", native_DrawTriangle3D, 10);

        // Model loading
        vm.registerNative("LoadModel", native_LoadModel, 1);
        vm.registerNative("UnloadModel", native_UnloadModel, 1);
        vm.registerNative("IsModelReady", native_IsModelReady, 1);
        vm.registerNative("DrawModel", native_DrawModel, 6);
        vm.registerNative("DrawModelWires", native_DrawModelWires, 6);

        // Mesh generation (returns Model)
        vm.registerNative("GenMeshCube", native_GenMeshCube, 3);
        vm.registerNative("GenMeshSphere", native_GenMeshSphere, 3);
        vm.registerNative("GenMeshPlane", native_GenMeshPlane, 4);
        vm.registerNative("GenMeshCylinder", native_GenMeshCylinder, 3);
        vm.registerNative("GenMeshCone", native_GenMeshCone, 3);
        vm.registerNative("GenMeshTorus", native_GenMeshTorus, 4);
        vm.registerNative("GenMeshKnot", native_GenMeshKnot, 4);

        // Model extras
        vm.registerNative("DrawCylinderWires", native_DrawCylinderWires, 8);
        vm.registerNative("SetMaterialTexture", native_SetMaterialTexture, 3);
        vm.registerNative("GetModelBoundingBox", native_GetModelBoundingBox, 1);
        vm.registerNative("DrawBillboard", native_DrawBillboard, 7);

        // 3D Collision
        vm.registerNative("CheckCollisionSpheres", native_CheckCollisionSpheres, 4);
        vm.registerNative("CheckCollisionBoxes", native_CheckCollisionBoxes, 4);
        vm.registerNative("CheckCollisionBoxSphere", native_CheckCollisionBoxSphere, 4);
        vm.registerNative("GetRayCollisionSphere", native_GetRayCollisionSphere, 4);
        vm.registerNative("GetRayCollisionBox", native_GetRayCollisionBox, 4);
        vm.registerNative("GetRayCollisionMesh", native_GetRayCollisionMesh, 5);
        vm.registerNative("GetRayCollisionTriangle", native_GetRayCollisionTriangle, 5);
        vm.registerNative("GetRayCollisionQuad", native_GetRayCollisionQuad, 6);

        // Material map type constants
        vm.addGlobal("MATERIAL_MAP_ALBEDO", vm.makeInt(MATERIAL_MAP_ALBEDO));
        vm.addGlobal("MATERIAL_MAP_METALNESS", vm.makeInt(MATERIAL_MAP_METALNESS));
        vm.addGlobal("MATERIAL_MAP_NORMAL", vm.makeInt(MATERIAL_MAP_NORMAL));
        vm.addGlobal("MATERIAL_MAP_ROUGHNESS", vm.makeInt(MATERIAL_MAP_ROUGHNESS));
        vm.addGlobal("MATERIAL_MAP_OCCLUSION", vm.makeInt(MATERIAL_MAP_OCCLUSION));
        vm.addGlobal("MATERIAL_MAP_EMISSION", vm.makeInt(MATERIAL_MAP_EMISSION));
        vm.addGlobal("MATERIAL_MAP_HEIGHT", vm.makeInt(MATERIAL_MAP_HEIGHT));
        vm.addGlobal("MATERIAL_MAP_CUBEMAP", vm.makeInt(MATERIAL_MAP_CUBEMAP));
        vm.addGlobal("MATERIAL_MAP_IRRADIANCE", vm.makeInt(MATERIAL_MAP_IRRADIANCE));
        vm.addGlobal("MATERIAL_MAP_PREFILTER", vm.makeInt(MATERIAL_MAP_PREFILTER));
        vm.addGlobal("MATERIAL_MAP_BRDF", vm.makeInt(MATERIAL_MAP_BRDF));
    }

}
