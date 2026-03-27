#include "bindings.hpp"
#include "raylib.h"

namespace RaylibBindings
{

    // ========================================
    // PIXEL
    // ========================================

    static int native_DrawPixel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("DrawPixel expects 3 arguments");
            return 0;
        }

        if (!args[2].isNativeStructInstance())
        {
            Error("DrawPixel expects Color");
            return 0;
        }

        int x = args[0].asNumber();
        int y = args[1].asNumber();

        auto *inst = args[2].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawPixel(x, y, *color);
        return 0;
    }

    // ========================================
    // LINE
    // ========================================

    static int native_DrawLine(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawLine expects 5 arguments");
            return 0;
        }

        if (!args[4].isNativeStructInstance())
        {
            Error("DrawLine expects Color");
            return 0;
        }

        int x1 = args[0].asNumber();
        int y1 = args[1].asNumber();
        int x2 = args[2].asNumber();
        int y2 = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawLine(x1, y1, x2, y2, *color);
        return 0;
    }

    static int native_DrawLineEx(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawLineEx expects 6 arguments");
            return 0;
        }

        if (!args[5].isNativeStructInstance())
        {
            Error("DrawLineEx expects Color");
            return 0;
        }

        Vector2 start = {(float)args[0].asNumber(), (float)args[1].asNumber()};
        Vector2 end = {(float)args[2].asNumber(), (float)args[3].asNumber()};

        auto *inst = args[5].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawLineEx(start, end, args[4].asNumber(), *color);
        return 0;
    }
    // ========================================
    // CIRCLE
    // ========================================

    static int native_DrawCircle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawCircle expects 4 arguments");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawCircle expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        float radius = args[2].asDouble();

        auto *inst = args[3].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawCircle(x, y, radius, *color);
        return 0;
    }

    static int native_DrawCircleLines(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawCircleLines expects 4 arguments");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawCircleLines expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        float radius = args[2].asDouble();

        auto *inst = args[3].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawCircleLines(x, y, radius, *color);
        return 0;
    }

    static int native_DrawCircleV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("DrawCircleV expects 3 arguments");
            return 0;
        }
        if (!args[2].isNativeStructInstance())
        {
            Error("DrawCircleV expects Color");
            return 0;
        }
        auto *posInst = args[0].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;

        float radius = args[1].asDouble();

        auto *colorInst = args[2].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCircleV(*pos, radius, *color);
        return 0;
    }

    // ========================================
    // RECTANGLE
    // ========================================

    static int native_DrawRectangle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawRectangle expects 5 arguments");
            return 0;
        }
        if (!args[4].isNativeStructInstance())
        {
            Error("DrawRectangle expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        int width = args[2].asNumber();
        int height = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawRectangle(x, y, width, height, *color);
        return 0;
    }

    static int native_DrawRectangleRec(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("DrawRectangleRec expects 2 arguments");
            return 0;
        }
        if (!args[0].isNativeStructInstance())
        {
            Error("DrawRectangleRec expects Rectangle");
            return 0;
        }
        if (!args[1].isNativeStructInstance())
        {
            Error("DrawRectangleRec expects Color");
            return 0;
        }
        auto *rectInst = args[0].asNativeStructInstance();
        Rectangle *rect = (Rectangle *)rectInst->data;

        auto *colorInst = args[1].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawRectangleRec(*rect, *color);
        return 0;
    }

    static int native_DrawRectangleLines(Interpreter *vm, int argc, Value *args)    
    {
        if (argc != 5)
        {
            Error("DrawRectangleLines expects 5 arguments");
            return 0;
        }
        if (!args[4].isNativeStructInstance())
        {
            Error("DrawRectangleLines expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        int width = args[2].asNumber();
        int height = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawRectangleLines(x, y, width, height, *color);
        return 0;
    }

    //DrawRectangleGradientV

    static int native_DrawRectangleGradientV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawRectangleGradientV expects 6 arguments");
            return 0;
        }
        if (!args[4].isNativeStructInstance() || !args[5].isNativeStructInstance())
        {
            Error("DrawRectangleGradientV expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        int width = args[2].asNumber();
        int height = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        auto *color2Inst = args[5].asNativeStructInstance();
        Color *color2 = (Color *)color2Inst->data;

        DrawRectangleGradientV(x, y,  width, height, *color, *color2);
        return 0;
    }

    static int native_DrawRectangleGradientH(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawRectangleGradientH expects 6 arguments");
            return 0;
        }
        if (!args[4].isNativeStructInstance() || !args[5].isNativeStructInstance())
        {
            Error("DrawRectangleGradientH expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        int width = args[2].asNumber();
        int height = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        auto *color2Inst = args[5].asNativeStructInstance();
        Color *color2 = (Color *)color2Inst->data;

        DrawRectangleGradientH(x, y,  width, height, *color, *color2);
        return 0;
    }

    //TRIANGLE

    static int native_DrawTriangle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawTriangle expects 4 arguments");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawTriangle expects Color");
            return 0;
        }
        auto *v1Inst = args[0].asNativeStructInstance();
        Vector2 *v1 = (Vector2 *)v1Inst->data;

      

         auto *v2Inst = args[1].asNativeStructInstance();
         Vector2 *v2 = (Vector2 *)v2Inst->data;

  

         auto *v3Inst = args[2].asNativeStructInstance();
         Vector2 *v3 = (Vector2 *)v3Inst->data;

      

         auto *colorInst = args[3].asNativeStructInstance();
         Color *color = (Color *)colorInst->data;

    

         DrawTriangle(*v1, *v2, *v3, *color);
        return 0;
    }

    // ========================================
    // LINE V
    // ========================================

    static int native_DrawLineV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isNativeStructInstance() || !args[1].isNativeStructInstance() || !args[2].isNativeStructInstance())
            return 0;
        Vector2 *start = (Vector2 *)args[0].asNativeStructInstance()->data;
        Vector2 *end = (Vector2 *)args[1].asNativeStructInstance()->data;
        Color *color = (Color *)args[2].asNativeStructInstance()->data;
        DrawLineV(*start, *end, *color);
        return 0;
    }

    static int native_DrawLineBezier(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isNativeStructInstance() || !args[1].isNativeStructInstance() || !args[3].isNativeStructInstance())
            return 0;
        Vector2 *start = (Vector2 *)args[0].asNativeStructInstance()->data;
        Vector2 *end = (Vector2 *)args[1].asNativeStructInstance()->data;
        float thick = args[2].asNumber();
        Color *color = (Color *)args[3].asNativeStructInstance()->data;
        DrawLineBezier(*start, *end, thick, *color);
        return 0;
    }

    // ========================================
    // ELLIPSE
    // ========================================

    static int native_DrawEllipse(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[4].isNativeStructInstance()) return 0;
        Color *color = (Color *)args[4].asNativeStructInstance()->data;
        DrawEllipse(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), *color);
        return 0;
    }

    static int native_DrawEllipseLines(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[4].isNativeStructInstance()) return 0;
        Color *color = (Color *)args[4].asNativeStructInstance()->data;
        DrawEllipseLines(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), *color);
        return 0;
    }

    // ========================================
    // RING
    // ========================================

    static int native_DrawRing(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7 || !args[0].isNativeStructInstance() || !args[6].isNativeStructInstance()) return 0;
        Vector2 *center = (Vector2 *)args[0].asNativeStructInstance()->data;
        Color *color = (Color *)args[6].asNativeStructInstance()->data;
        DrawRing(*center, args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber(), args[5].asNumber(), *color);
        return 0;
    }

    static int native_DrawRingLines(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7 || !args[0].isNativeStructInstance() || !args[6].isNativeStructInstance()) return 0;
        Vector2 *center = (Vector2 *)args[0].asNativeStructInstance()->data;
        Color *color = (Color *)args[6].asNativeStructInstance()->data;
        DrawRingLines(*center, args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber(), args[5].asNumber(), *color);
        return 0;
    }

    // ========================================
    // RECTANGLE PRO / ROUNDED
    // ========================================

    static int native_DrawRectanglePro(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isNativeStructInstance() || !args[1].isNativeStructInstance() || !args[3].isNativeStructInstance()) return 0;
        Rectangle *rec = (Rectangle *)args[0].asNativeStructInstance()->data;
        Vector2 *origin = (Vector2 *)args[1].asNativeStructInstance()->data;
        Color *color = (Color *)args[3].asNativeStructInstance()->data;
        DrawRectanglePro(*rec, *origin, args[2].asNumber(), *color);
        return 0;
    }

    static int native_DrawRectangleRounded(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isNativeStructInstance() || !args[3].isNativeStructInstance()) return 0;
        Rectangle *rec = (Rectangle *)args[0].asNativeStructInstance()->data;
        Color *color = (Color *)args[3].asNativeStructInstance()->data;
        DrawRectangleRounded(*rec, args[1].asNumber(), args[2].asNumber(), *color);
        return 0;
    }

    static int native_DrawRectangleRoundedLines(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[0].isNativeStructInstance() || !args[4].isNativeStructInstance()) return 0;
        Rectangle *rec = (Rectangle *)args[0].asNativeStructInstance()->data;
        Color *color = (Color *)args[4].asNativeStructInstance()->data;
        DrawRectangleRoundedLines(*rec, args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), *color);
        return 0;
    }

    static int native_DrawRectangleLinesEx(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isNativeStructInstance() || !args[2].isNativeStructInstance()) return 0;
        Rectangle *rec = (Rectangle *)args[0].asNativeStructInstance()->data;
        Color *color = (Color *)args[2].asNativeStructInstance()->data;
        DrawRectangleLinesEx(*rec, args[1].asNumber(), *color);
        return 0;
    }

    // ========================================
    // TRIANGLE LINES / POLY
    // ========================================

    static int native_DrawTriangleLines(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) return 0;
        Vector2 *v1 = (Vector2 *)args[0].asNativeStructInstance()->data;
        Vector2 *v2 = (Vector2 *)args[1].asNativeStructInstance()->data;
        Vector2 *v3 = (Vector2 *)args[2].asNativeStructInstance()->data;
        Color *color = (Color *)args[3].asNativeStructInstance()->data;
        DrawTriangleLines(*v1, *v2, *v3, *color);
        return 0;
    }

    static int native_DrawPoly(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[0].isNativeStructInstance() || !args[4].isNativeStructInstance()) return 0;
        Vector2 *center = (Vector2 *)args[0].asNativeStructInstance()->data;
        Color *color = (Color *)args[4].asNativeStructInstance()->data;
        DrawPoly(*center, args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), *color);
        return 0;
    }

    static int native_DrawPolyLines(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[0].isNativeStructInstance() || !args[4].isNativeStructInstance()) return 0;
        Vector2 *center = (Vector2 *)args[0].asNativeStructInstance()->data;
        Color *color = (Color *)args[4].asNativeStructInstance()->data;
        DrawPolyLines(*center, args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), *color);
        return 0;
    }

    // ========================================
    // CIRCLE SECTOR / GRADIENT
    // ========================================

    static int native_DrawCircleSector(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6 || !args[0].isNativeStructInstance() || !args[5].isNativeStructInstance()) return 0;
        Vector2 *center = (Vector2 *)args[0].asNativeStructInstance()->data;
        Color *color = (Color *)args[5].asNativeStructInstance()->data;
        DrawCircleSector(*center, args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber(), *color);
        return 0;
    }

    static int native_DrawCircleGradient(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[3].isNativeStructInstance() || !args[4].isNativeStructInstance()) return 0;
        Color *c1 = (Color *)args[3].asNativeStructInstance()->data;
        Color *c2 = (Color *)args[4].asNativeStructInstance()->data;
        DrawCircleGradient(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), *c1, *c2);
        return 0;
    }

    // ========================================
    // COLLISION CHECKS
    // ========================================

    static int native_CheckCollisionRecs(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        Rectangle *r1 = (Rectangle *)args[0].asNativeStructInstance()->data;
        Rectangle *r2 = (Rectangle *)args[1].asNativeStructInstance()->data;
        vm->pushBool(CheckCollisionRecs(*r1, *r2));
        return 1;
    }

    static int native_CheckCollisionCircles(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) return 0;
        Vector2 *c1 = (Vector2 *)args[0].asNativeStructInstance()->data;
        Vector2 *c2 = (Vector2 *)args[2].asNativeStructInstance()->data;
        vm->pushBool(CheckCollisionCircles(*c1, args[1].asNumber(), *c2, args[3].asNumber()));
        return 1;
    }

    static int native_CheckCollisionCircleRec(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        Vector2 *center = (Vector2 *)args[0].asNativeStructInstance()->data;
        Rectangle *rec = (Rectangle *)args[2].asNativeStructInstance()->data;
        vm->pushBool(CheckCollisionCircleRec(*center, args[1].asNumber(), *rec));
        return 1;
    }

    static int native_CheckCollisionPointRec(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        Vector2 *point = (Vector2 *)args[0].asNativeStructInstance()->data;
        Rectangle *rec = (Rectangle *)args[1].asNativeStructInstance()->data;
        vm->pushBool(CheckCollisionPointRec(*point, *rec));
        return 1;
    }

    static int native_CheckCollisionPointCircle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        Vector2 *point = (Vector2 *)args[0].asNativeStructInstance()->data;
        Vector2 *center = (Vector2 *)args[1].asNativeStructInstance()->data;
        vm->pushBool(CheckCollisionPointCircle(*point, *center, args[2].asNumber()));
        return 1;
    }

    static int native_CheckCollisionPointTriangle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4) return 0;
        Vector2 *p = (Vector2 *)args[0].asNativeStructInstance()->data;
        Vector2 *p1 = (Vector2 *)args[1].asNativeStructInstance()->data;
        Vector2 *p2 = (Vector2 *)args[2].asNativeStructInstance()->data;
        Vector2 *p3 = (Vector2 *)args[3].asNativeStructInstance()->data;
        vm->pushBool(CheckCollisionPointTriangle(*p, *p1, *p2, *p3));
        return 1;
    }

    static int native_GetCollisionRec(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        Rectangle *r1 = (Rectangle *)args[0].asNativeStructInstance()->data;
        Rectangle *r2 = (Rectangle *)args[1].asNativeStructInstance()->data;
        Rectangle result = GetCollisionRec(*r1, *r2);
        vm->pushDouble(result.x);
        vm->pushDouble(result.y);
        vm->pushDouble(result.width);
        vm->pushDouble(result.height);
        return 4;
    }

    static int native_CheckCollisionPointPoly(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        Vector2 *point = (Vector2 *)args[0].asNativeStructInstance()->data;
        if (!args[1].isArray()) return 0;
        ArrayInstance *arr = args[1].asArray();
        int count = arr->values.size();
        if (count == 0) { vm->pushBool(false); return 1; }
        Vector2 *points = new Vector2[count];
        for (int i = 0; i < count; i++) {
            Vector2 *v = (Vector2 *)arr->values[i].asNativeStructInstance()->data;
            points[i] = *v;
        }
        vm->pushBool(CheckCollisionPointPoly(*point, points, count));
        delete[] points;
        return 1;
    }

    // ========================================
    // REGISTER SHAPES
    // ========================================

    void register_shapes(Interpreter &vm)
    {
        vm.registerNative("DrawPixel", native_DrawPixel, 3);
        vm.registerNative("DrawTriangle", native_DrawTriangle, 4);
        vm.registerNative("DrawTriangleLines", native_DrawTriangleLines, 4);
        vm.registerNative("DrawLine", native_DrawLine, 5);
        vm.registerNative("DrawLineV", native_DrawLineV, 3);
        vm.registerNative("DrawLineEx", native_DrawLineEx, 6);
        vm.registerNative("DrawLineBezier", native_DrawLineBezier, 4);
        vm.registerNative("DrawCircle", native_DrawCircle, 4);
        vm.registerNative("DrawCircleV", native_DrawCircleV, 3);
        vm.registerNative("DrawCircleLines", native_DrawCircleLines, 4);
        vm.registerNative("DrawCircleSector", native_DrawCircleSector, 6);
        vm.registerNative("DrawCircleGradient", native_DrawCircleGradient, 5);
        vm.registerNative("DrawEllipse", native_DrawEllipse, 5);
        vm.registerNative("DrawEllipseLines", native_DrawEllipseLines, 5);
        vm.registerNative("DrawRing", native_DrawRing, 7);
        vm.registerNative("DrawRingLines", native_DrawRingLines, 7);
        vm.registerNative("DrawRectangle", native_DrawRectangle, 5);
        vm.registerNative("DrawRectangleRec", native_DrawRectangleRec, 2);
        vm.registerNative("DrawRectanglePro", native_DrawRectanglePro, 4);
        vm.registerNative("DrawRectangleLines", native_DrawRectangleLines, 5);
        vm.registerNative("DrawRectangleLinesEx", native_DrawRectangleLinesEx, 3);
        vm.registerNative("DrawRectangleRounded", native_DrawRectangleRounded, 4);
        vm.registerNative("DrawRectangleRoundedLines", native_DrawRectangleRoundedLines, 5);
        vm.registerNative("DrawRectangleGradientV", native_DrawRectangleGradientV, 6);
        vm.registerNative("DrawRectangleGradientH", native_DrawRectangleGradientH, 6);
        vm.registerNative("DrawPoly", native_DrawPoly, 5);
        vm.registerNative("DrawPolyLines", native_DrawPolyLines, 5);
           // Collision
        vm.registerNative("CheckCollisionRecs", native_CheckCollisionRecs, 2);
        vm.registerNative("CheckCollisionCircles", native_CheckCollisionCircles, 4);
        vm.registerNative("CheckCollisionCircleRec", native_CheckCollisionCircleRec, 3);
        vm.registerNative("CheckCollisionPointRec", native_CheckCollisionPointRec, 2);
        vm.registerNative("CheckCollisionPointCircle", native_CheckCollisionPointCircle, 3);
        vm.registerNative("CheckCollisionPointTriangle", native_CheckCollisionPointTriangle, 4);
        vm.registerNative("GetCollisionRec", native_GetCollisionRec, 2);
        vm.registerNative("CheckCollisionPointPoly", native_CheckCollisionPointPoly, 2);
    }

}
