#include "bindings.hpp"
#include "raylib.h"
#include "rlgl.h"
#include <vector>

namespace RaylibBindings
{

    // ========================================
    // LOAD/UNLOAD
    // ========================================

    static int native_LoadTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("LoadTexture expects 1 argument");
            return 0;
        }
        if (!args[0].isString())
        {
            Error("LoadTexture expects string");
            return 0;
        }
        const char *filename = args[0].asStringChars();

        Texture2D tex = LoadTexture(filename);
        Texture2D *texPtr = new Texture2D(tex);

        vm->push(vm->makePointer(texPtr));

        return 1;
    }

    static int native_UnloadTexture(Interpreter *vm, int argc, Value *args)
    {
        Texture2D *tex = (Texture2D *)args[0].asPointer();

        UnloadTexture(*tex);
        delete tex;

        return 0;
    }

    // ========================================
    // DRAW TEXTURE
    // ========================================

    static int native_DrawTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawTexture expects 4 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawTexture expects Texture2D");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawTexture expects Color");
            return 0;
        }
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        int x = args[1].asNumber();
        int y = args[2].asNumber();

        auto *colorInst = args[3].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTexture(*tex, x, y, *tint);
        return 0;
    }

    static int native_DrawTextureV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("DrawTextureV expects 3 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawTextureV expects Texture2D");
            return 0;
        }
        if (!args[1].isNativeStructInstance())
        {
            Error("DrawTextureV expects Vector2");
            return 0;
        }
        if (!args[2].isNativeStructInstance())
        {
            Error("DrawTextureV expects Color");
            return 0;
        }

        Texture2D *tex = (Texture2D *)args[0].asPointer();

        auto *posInst = args[1].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;

        auto *colorInst = args[2].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTextureV(*tex, *pos, *tint);
        return 0;
    }

    static int native_DrawTextureEx(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawTextureEx expects 6 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawTextureEx expects Texture2D");
            return 0;
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawTextureEx expects Color");
            return 0;
        }
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        int x = (int)args[1].asNumber();
        int y = (int)args[2].asNumber();
        double rotation = args[3].asNumber();
        double scale = args[4].asNumber();
        Vector2 position = {(float)x, (float)y};

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTextureEx(*tex, position, rotation, scale, *tint);
        return 0;
    }

    static  int native_DrawTexturePro(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawTexturePro expects 5 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawTexturePro expects Texture2D");
            return 0;
        }
        if (!args[1].isNativeStructInstance())
        {
            Error("DrawTexturePro expects Rectangle");
            return 0;
        }
        if (!args[2].isNativeStructInstance())
        {
            Error("DrawTexturePro expects Rectangle");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawTexturePro expects Vector2");
            return 0;
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawTexturePro expects Color");
            return 0;
        }

        Texture2D *tex = (Texture2D *)args[0].asPointer();

        auto *sourceInst = args[1].asNativeStructInstance();
        Rectangle *source = (Rectangle *)sourceInst->data;
 

        auto *destInst = args[2].asNativeStructInstance();
        Rectangle *dest = (Rectangle *)destInst->data;

        auto *originInst = args[3].asNativeStructInstance();
        Vector2 *origin = (Vector2 *)originInst->data;


        double rotation = args[4].asNumber();
        

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTexturePro(*tex, *source, *dest, *origin, rotation, *tint);
        return 0;
        
    }

    // ========================================
    // RENDER TEXTURES
    // ========================================

    static int native_LoadRenderTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("LoadRenderTexture expects width, height");
            return 0;
        }

        int w = (int)args[0].asNumber();
        int h = (int)args[1].asNumber();

        // IMPORTANTE: Guardamos a RenderTexture2D completa, não só a textura!
        RenderTexture2D rt = LoadRenderTexture(w, h);
        RenderTexture2D *rtPtr = new RenderTexture2D(rt);

        vm->push(vm->makePointer(rtPtr));
        return 1;
    }

    static int native_UnloadRenderTexture(Interpreter *vm, int argc, Value *args)
    {
        if (!args[0].isPointer())
            return 0;
        RenderTexture2D *rt = (RenderTexture2D *)args[0].asPointer();
        UnloadRenderTexture(*rt);
        delete rt;
        return 0;
    }

    static int native_GetRenderTextureTexture(Interpreter *vm, int argc, Value *args)
    {
        RenderTexture2D *rt = (RenderTexture2D *)args[0].asPointer();
        vm->push(vm->makePointer(&(rt->texture)));
        return 1;
    }

    static int native_BeginTextureMode(Interpreter *vm, int argc, Value *args)
    {
        if (!args[0].isPointer())
        {
            vm->runtimeError("BeginTextureMode expects RenderTexture2D");
            return 0;
        }

        RenderTexture2D *rt = (RenderTexture2D *)args[0].asPointer();
        BeginTextureMode(*rt);
        return 0;
    }

    static int native_EndTextureMode(Interpreter *vm, int argc, Value *args)
    {
        EndTextureMode();
        return 0;
    }

    // LoadTextureFromImage

    // ========================================
    // IMAGE
    // ========================================

    static int native_LoadImage(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString()) return 0;
        Image img = LoadImage(args[0].asStringChars());
        Image *ptr = new Image(img);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_UnloadImage(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        UnloadImage(*img);
        delete img;
        return 0;
    }

    static int native_LoadTextureFromImage(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Texture2D tex = LoadTextureFromImage(*img);
        Texture2D *ptr = new Texture2D(tex);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_IsTextureReady(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        vm->pushBool(IsTextureReady(*tex));
        return 1;
    }

    static int native_SetTextureFilter(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        SetTextureFilter(*tex, args[1].asNumber());
        return 0;
    }

    static int native_SetTextureWrap(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        SetTextureWrap(*tex, args[1].asNumber());
        return 0;
    }

    static int native_DrawTextureRec(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isPointer() || !args[1].isNativeStructInstance() ||
            !args[2].isNativeStructInstance() || !args[3].isNativeStructInstance()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        Rectangle *source = (Rectangle *)args[1].asNativeStructInstance()->data;
        Vector2 *pos = (Vector2 *)args[2].asNativeStructInstance()->data;
        Color *tint = (Color *)args[3].asNativeStructInstance()->data;
        DrawTextureRec(*tex, *source, *pos, *tint);
        return 0;
    }

    static int native_GenImageColor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[2].isNativeStructInstance()) return 0;
        Color *color = (Color *)args[2].asNativeStructInstance()->data;
        Image img = GenImageColor(args[0].asNumber(), args[1].asNumber(), *color);
        Image *ptr = new Image(img);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_GenImageChecked(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6 || !args[4].isNativeStructInstance() || !args[5].isNativeStructInstance()) return 0;
        Color *c1 = (Color *)args[4].asNativeStructInstance()->data;
        Color *c2 = (Color *)args[5].asNativeStructInstance()->data;
        Image img = GenImageChecked(args[0].asNumber(), args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), *c1, *c2);
        Image *ptr = new Image(img);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_ImageFlipVertical(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageFlipVertical(img);
        return 0;
    }

    static int native_ImageFlipHorizontal(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageFlipHorizontal(img);
        return 0;
    }

    static int native_ImageResize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageResize(img, args[1].asNumber(), args[2].asNumber());
        return 0;
    }

    static int native_ExportImage(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer() || !args[1].isString()) return 0;
        Image *img = (Image *)args[0].asPointer();
        vm->pushBool(ExportImage(*img, args[1].asStringChars()));
        return 1;
    }

    static int native_LoadImageFromScreen(Interpreter *vm, int argc, Value *args)
    {
        Image img = LoadImageFromScreen();
        Image *ptr = new Image(img);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    // ========================================
    // SHADER
    // ========================================

    static int native_LoadShader(Interpreter *vm, int argc, Value *args)
    {
        const char *vs = (argc >= 1 && args[0].isString()) ? args[0].asStringChars() : nullptr;
        const char *fs = (argc >= 2 && args[1].isString()) ? args[1].asStringChars() : nullptr;
        Shader shader = LoadShader(vs, fs);
        Shader *ptr = new Shader(shader);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_LoadShaderFromMemory(Interpreter *vm, int argc, Value *args)
    {
        const char *vs = (argc >= 1 && args[0].isString()) ? args[0].asStringChars() : nullptr;
        const char *fs = (argc >= 2 && args[1].isString()) ? args[1].asStringChars() : nullptr;
        Shader shader = LoadShaderFromMemory(vs, fs);
        Shader *ptr = new Shader(shader);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_UnloadShader(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        UnloadShader(*shader);
        delete shader;
        return 0;
    }

    static int native_IsShaderReady(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        vm->pushBool(IsShaderReady(*shader));
        return 1;
    }

    static int native_GetShaderLocation(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer() || !args[1].isString()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        vm->pushInt(GetShaderLocation(*shader, args[1].asStringChars()));
        return 1;
    }

    static int native_SetShaderValueFloat(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isPointer()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        int loc = args[1].asNumber();
        float val = args[2].asNumber();
        SetShaderValue(*shader, loc, &val, SHADER_UNIFORM_FLOAT);
        return 0;
    }

    static int native_SetShaderValueVec2(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isPointer()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        int loc = args[1].asNumber();
        float val[2] = {(float)args[2].asNumber(), (float)args[3].asNumber()};
        SetShaderValue(*shader, loc, val, SHADER_UNIFORM_VEC2);
        return 0;
    }

    static int native_SetShaderValueVec3(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[0].isPointer()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        int loc = args[1].asNumber();
        float val[3] = {(float)args[2].asNumber(), (float)args[3].asNumber(), (float)args[4].asNumber()};
        SetShaderValue(*shader, loc, val, SHADER_UNIFORM_VEC3);
        return 0;
    }

    static int native_SetShaderValueVec4(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6 || !args[0].isPointer()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        int loc = args[1].asNumber();
        float val[4] = {(float)args[2].asNumber(), (float)args[3].asNumber(), (float)args[4].asNumber(), (float)args[5].asNumber()};
        SetShaderValue(*shader, loc, val, SHADER_UNIFORM_VEC4);
        return 0;
    }

    static int native_SetShaderValueInt(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isPointer()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        int loc = args[1].asNumber();
        int val = args[2].asNumber();
        SetShaderValue(*shader, loc, &val, SHADER_UNIFORM_INT);
        return 0;
    }

    static int native_BeginShaderMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        BeginShaderMode(*shader);
        return 0;
    }

    static int native_EndShaderMode(Interpreter *vm, int argc, Value *args)
    {
        EndShaderMode();
        return 0;
    }

    // ========================================
    // COLOR HELPERS
    // ========================================

    static int native_ColorFromHSV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        Color c = ColorFromHSV(args[0].asNumber(), args[1].asNumber(), args[2].asNumber());
        vm->pushInt(c.r);
        vm->pushInt(c.g);
        vm->pushInt(c.b);
        vm->pushInt(c.a);
        return 4;
    }

    static int native_GetColor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        Color c = GetColor((unsigned int)args[0].asNumber());
        vm->pushInt(c.r);
        vm->pushInt(c.g);
        vm->pushInt(c.b);
        vm->pushInt(c.a);
        return 4;
    }

    static int native_ColorAlpha(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNativeStructInstance()) return 0;
        Color *color = (Color *)args[0].asNativeStructInstance()->data;
        Color c = ColorAlpha(*color, args[1].asNumber());
        color->r = c.r; color->g = c.g; color->b = c.b; color->a = c.a;
        return 0;
    }

    // ========================================
    // MISC
    // ========================================

    static int native_TakeScreenshot(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString()) return 0;
        TakeScreenshot(args[0].asStringChars());
        return 0;
    }

    static int native_SetRandomSeed(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1) return 0;
        SetRandomSeed((unsigned int)args[0].asNumber());
        return 0;
    }

    static int native_GetRandomValue(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2) return 0;
        vm->pushInt(GetRandomValue(args[0].asNumber(), args[1].asNumber()));
        return 1;
    }

    // ========================================
    // COLOR HELPERS (EXTRA)
    // ========================================

    static int native_ColorToInt(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNativeStructInstance()) return 0;
        Color *c = (Color *)args[0].asNativeStructInstance()->data;
        vm->pushInt(ColorToInt(*c));
        return 1;
    }

    static int native_ColorNormalize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNativeStructInstance()) return 0;
        Color *c = (Color *)args[0].asNativeStructInstance()->data;
        Vector4 n = ColorNormalize(*c);
        vm->pushFloat(n.x);
        vm->pushFloat(n.y);
        vm->pushFloat(n.z);
        vm->pushFloat(n.w);
        return 4;
    }

    static int native_ColorToHSV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNativeStructInstance()) return 0;
        Color *c = (Color *)args[0].asNativeStructInstance()->data;
        Vector3 hsv = ColorToHSV(*c);
        vm->pushFloat(hsv.x);
        vm->pushFloat(hsv.y);
        vm->pushFloat(hsv.z);
        return 3;
    }

    static int native_ColorTint(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNativeStructInstance() || !args[1].isNativeStructInstance()) return 0;
        Color *c = (Color *)args[0].asNativeStructInstance()->data;
        Color *tint = (Color *)args[1].asNativeStructInstance()->data;
        Color result = ColorTint(*c, *tint);
        c->r = result.r; c->g = result.g; c->b = result.b; c->a = result.a;
        return 0;
    }

    static int native_ColorBrightness(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNativeStructInstance()) return 0;
        Color *c = (Color *)args[0].asNativeStructInstance()->data;
        Color result = ColorBrightness(*c, args[1].asNumber());
        c->r = result.r; c->g = result.g; c->b = result.b; c->a = result.a;
        return 0;
    }

    static int native_ColorContrast(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNativeStructInstance()) return 0;
        Color *c = (Color *)args[0].asNativeStructInstance()->data;
        Color result = ColorContrast(*c, args[1].asNumber());
        c->r = result.r; c->g = result.g; c->b = result.b; c->a = result.a;
        return 0;
    }

    static int native_SetShaderValueTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isPointer() || !args[2].isPointer()) return 0;
        Shader *shader = (Shader *)args[0].asPointer();
        int loc = args[1].asInt();
        Texture2D *tex = (Texture2D *)args[2].asPointer();
        SetShaderValueTexture(*shader, loc, *tex);
        return 0;
    }

    // ========================================
    // IMAGE MANIPULATION
    // ========================================

    static int native_IsImageReady(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        vm->pushBool(IsImageReady(*img));
        return 1;
    }

    static int native_ImageCopy(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Image copy = ImageCopy(*img);
        Image *ptr = new Image(copy);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_ImageFromImage(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Rectangle *rec = (Rectangle *)args[1].asNativeStructInstance()->data;
        Image sub = ImageFromImage(*img, *rec);
        Image *ptr = new Image(sub);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_ImageCrop(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Rectangle *rec = (Rectangle *)args[1].asNativeStructInstance()->data;
        ImageCrop(img, *rec);
        return 0;
    }

    static int native_ImageAlphaCrop(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageAlphaCrop(img, (float)args[1].asNumber());
        return 0;
    }

    static int native_ImageAlphaClear(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Color *c = (Color *)args[1].asNativeStructInstance()->data;
        ImageAlphaClear(img, *c, (float)args[2].asNumber());
        return 0;
    }

    static int native_ImageAlphaPremultiply(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageAlphaPremultiply(img);
        return 0;
    }

    static int native_ImageBlurGaussian(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageBlurGaussian(img, args[1].asInt());
        return 0;
    }

    static int native_ImageResizeNN(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageResizeNN(img, args[1].asInt(), args[2].asInt());
        return 0;
    }

    static int native_ImageResizeCanvas(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Color *fill = (Color *)args[5].asNativeStructInstance()->data;
        ImageResizeCanvas(img, args[1].asInt(), args[2].asInt(), args[3].asInt(), args[4].asInt(), *fill);
        return 0;
    }

    static int native_ImageMipmaps(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageMipmaps(img);
        return 0;
    }

    static int native_ImageFormat(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageFormat(img, args[1].asInt());
        return 0;
    }

    static int native_ImageRotate(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        ImageRotate(img, args[1].asInt());
        return 0;
    }

    static int native_ImageRotateCW(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        ImageRotateCW((Image *)args[0].asPointer());
        return 0;
    }

    static int native_ImageRotateCCW(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        ImageRotateCCW((Image *)args[0].asPointer());
        return 0;
    }

    // ========================================
    // IMAGE COLOR OPERATIONS
    // ========================================

    static int native_ImageColorTint(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Color *c = (Color *)args[1].asNativeStructInstance()->data;
        ImageColorTint(img, *c);
        return 0;
    }

    static int native_ImageColorInvert(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        ImageColorInvert((Image *)args[0].asPointer());
        return 0;
    }

    static int native_ImageColorGrayscale(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        ImageColorGrayscale((Image *)args[0].asPointer());
        return 0;
    }

    static int native_ImageColorContrast(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        ImageColorContrast((Image *)args[0].asPointer(), (float)args[1].asNumber());
        return 0;
    }

    static int native_ImageColorBrightness(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        ImageColorBrightness((Image *)args[0].asPointer(), args[1].asInt());
        return 0;
    }

    static int native_ImageColorReplace(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Color *c1 = (Color *)args[1].asNativeStructInstance()->data;
        Color *c2 = (Color *)args[2].asNativeStructInstance()->data;
        ImageColorReplace(img, *c1, *c2);
        return 0;
    }

    static int native_GetImageColor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Color c = GetImageColor(*img, args[1].asInt(), args[2].asInt());
        vm->pushInt(c.r);
        vm->pushInt(c.g);
        vm->pushInt(c.b);
        vm->pushInt(c.a);
        return 4;
    }

    static int native_GetImageAlphaBorder(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        Rectangle r = GetImageAlphaBorder(*img, (float)args[1].asNumber());
        vm->pushDouble(r.x);
        vm->pushDouble(r.y);
        vm->pushDouble(r.width);
        vm->pushDouble(r.height);
        return 4;
    }

    static int native_LoadImageFromTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        Image img = LoadImageFromTexture(*tex);
        Image *ptr = new Image(img);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    // ========================================
    // IMAGE GENERATION
    // ========================================

    static int native_GenImageGradientLinear(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5) return 0;
        Color *start = (Color *)args[3].asNativeStructInstance()->data;
        Color *end = (Color *)args[4].asNativeStructInstance()->data;
        Image img = GenImageGradientLinear(args[0].asInt(), args[1].asInt(), args[2].asInt(), *start, *end);
        vm->push(vm->makePointer(new Image(img)));
        return 1;
    }

    static int native_GenImageGradientRadial(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5) return 0;
        Color *inner = (Color *)args[3].asNativeStructInstance()->data;
        Color *outer = (Color *)args[4].asNativeStructInstance()->data;
        Image img = GenImageGradientRadial(args[0].asInt(), args[1].asInt(), (float)args[2].asNumber(), *inner, *outer);
        vm->push(vm->makePointer(new Image(img)));
        return 1;
    }

    static int native_GenImageGradientSquare(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5) return 0;
        Color *inner = (Color *)args[3].asNativeStructInstance()->data;
        Color *outer = (Color *)args[4].asNativeStructInstance()->data;
        Image img = GenImageGradientSquare(args[0].asInt(), args[1].asInt(), (float)args[2].asNumber(), *inner, *outer);
        vm->push(vm->makePointer(new Image(img)));
        return 1;
    }

    static int native_GenImageWhiteNoise(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        Image img = GenImageWhiteNoise(args[0].asInt(), args[1].asInt(), (float)args[2].asNumber());
        vm->push(vm->makePointer(new Image(img)));
        return 1;
    }

    static int native_GenImagePerlinNoise(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5) return 0;
        Image img = GenImagePerlinNoise(args[0].asInt(), args[1].asInt(), args[2].asInt(), args[3].asInt(), (float)args[4].asNumber());
        vm->push(vm->makePointer(new Image(img)));
        return 1;
    }

    static int native_GenImageCellular(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3) return 0;
        Image img = GenImageCellular(args[0].asInt(), args[1].asInt(), args[2].asInt());
        vm->push(vm->makePointer(new Image(img)));
        return 1;
    }

    // ========================================
    // IMAGE DRAWING (draw primitives onto images)
    // ========================================

    static int native_ImageClearBackground(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer()) return 0;
        Color *c = (Color *)args[1].asNativeStructInstance()->data;
        ImageClearBackground((Image *)args[0].asPointer(), *c);
        return 0;
    }

    static int native_ImageDrawPixel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isPointer()) return 0;
        Color *c = (Color *)args[3].asNativeStructInstance()->data;
        ImageDrawPixel((Image *)args[0].asPointer(), args[1].asInt(), args[2].asInt(), *c);
        return 0;
    }

    static int native_ImageDrawLine(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6 || !args[0].isPointer()) return 0;
        Color *c = (Color *)args[5].asNativeStructInstance()->data;
        ImageDrawLine((Image *)args[0].asPointer(), args[1].asInt(), args[2].asInt(), args[3].asInt(), args[4].asInt(), *c);
        return 0;
    }

    static int native_ImageDrawCircle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[0].isPointer()) return 0;
        Color *c = (Color *)args[4].asNativeStructInstance()->data;
        ImageDrawCircle((Image *)args[0].asPointer(), args[1].asInt(), args[2].asInt(), args[3].asInt(), *c);
        return 0;
    }

    static int native_ImageDrawCircleLines(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[0].isPointer()) return 0;
        Color *c = (Color *)args[4].asNativeStructInstance()->data;
        ImageDrawCircleLines((Image *)args[0].asPointer(), args[1].asInt(), args[2].asInt(), args[3].asInt(), *c);
        return 0;
    }

    static int native_ImageDrawRectangle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6 || !args[0].isPointer()) return 0;
        Color *c = (Color *)args[5].asNativeStructInstance()->data;
        ImageDrawRectangle((Image *)args[0].asPointer(), args[1].asInt(), args[2].asInt(), args[3].asInt(), args[4].asInt(), *c);
        return 0;
    }

    static int native_ImageDrawRectangleRec(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3 || !args[0].isPointer()) return 0;
        Rectangle *rec = (Rectangle *)args[1].asNativeStructInstance()->data;
        Color *c = (Color *)args[2].asNativeStructInstance()->data;
        ImageDrawRectangleRec((Image *)args[0].asPointer(), *rec, *c);
        return 0;
    }

    static int native_ImageDrawRectangleLines(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isPointer()) return 0;
        Rectangle *rec = (Rectangle *)args[1].asNativeStructInstance()->data;
        Color *c = (Color *)args[3].asNativeStructInstance()->data;
        ImageDrawRectangleLines((Image *)args[0].asPointer(), *rec, args[2].asInt(), *c);
        return 0;
    }

    static int native_ImageDraw(Interpreter *vm, int argc, Value *args)
    {
        // ImageDraw(dst, src, srcRec, dstRec, tint)
        if (argc != 5 || !args[0].isPointer() || !args[1].isPointer()) return 0;
        Image *dst = (Image *)args[0].asPointer();
        Image *src = (Image *)args[1].asPointer();
        Rectangle *srcRec = (Rectangle *)args[2].asNativeStructInstance()->data;
        Rectangle *dstRec = (Rectangle *)args[3].asNativeStructInstance()->data;
        Color *tint = (Color *)args[4].asNativeStructInstance()->data;
        ImageDraw(dst, *src, *srcRec, *dstRec, *tint);
        return 0;
    }

    static int native_ImageDrawText(Interpreter *vm, int argc, Value *args)
    {
        // ImageDrawText(dst, text, x, y, fontSize, color)
        if (argc != 6 || !args[0].isPointer()) return 0;
        Color *c = (Color *)args[5].asNativeStructInstance()->data;
        ImageDrawText((Image *)args[0].asPointer(), args[1].asStringChars(), args[2].asInt(), args[3].asInt(), args[4].asInt(), *c);
        return 0;
    }

    // ========================================
    // IMAGE SIZE QUERY
    // ========================================

    static int native_GetImageSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Image *img = (Image *)args[0].asPointer();
        vm->pushInt(img->width);
        vm->pushInt(img->height);
        return 2;
    }

    // ========================================
    // TEXTURED POLYGON DRAWING
    // ========================================

    // Helper: extract float pairs from buffer, typedarray, or BuLang array
    static const float *resolveFloat2Array(const Value &val, int &countOut, std::vector<float> &scratch)
    {
        // Buffer: zero-copy, data is float pairs
        if (val.isBuffer())
        {
            BufferInstance *buf = val.asBuffer();
            if (!buf || buf->count < 2) return nullptr;
            countOut = (buf->count * buf->elementSize) / (int)(sizeof(float) * 2);
            return (const float *)buf->data;
        }
        // TypedArray: zero-copy
        const void *ptr = nullptr;
        if (getBuiltinTypedArrayData(val, &ptr) && ptr)
        {
            // Can't know size from typedarray alone, caller must provide count
            return (const float *)ptr;
        }
        // BuLang array of numbers: [x0, y0, x1, y1, ...]
        if (val.isArray())
        {
            ArrayInstance *arr = val.asArray();
            if (!arr) return nullptr;
            int n = (int)arr->values.size();
            if (n < 2) return nullptr;
            countOut = n / 2;
            scratch.resize(n);
            for (int i = 0; i < n; i++)
                scratch[i] = (float)arr->values[i].asNumber();
            return scratch.data();
        }
        return nullptr;
    }

    // DrawTexturePoly(texture, vertices, texcoords, vertexCount, color)
    //   vertices:  buffer/array of float pairs [x0,y0, x1,y1, ...]
    //   texcoords: buffer/array of float pairs [u0,v0, u1,v1, ...]
    //   vertexCount: int (number of vertices)
    //   Draws as triangle fan (first vertex = center)
    static int native_DrawTexturePoly(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        int vertCount = args[3].asInt();
        if (vertCount < 3) return 0;
        Color *tint = (Color *)args[4].asNativeStructInstance()->data;

        int vCount = 0, tCount = 0;
        std::vector<float> vScratch, tScratch;
        const float *verts = resolveFloat2Array(args[1], vCount, vScratch);
        const float *uvs = resolveFloat2Array(args[2], tCount, tScratch);
        if (!verts || !uvs) return 0;

        // Draw textured triangle fan via rlgl
        rlSetTexture(tex->id);
        rlBegin(RL_QUADS);
            rlColor4ub(tint->r, tint->g, tint->b, tint->a);

            for (int i = 1; i < vertCount - 1; i++)
            {
                // Triangle: center(0), i, i+1
                // Vertex 0 (center)
                rlTexCoord2f(uvs[0], uvs[1]);
                rlVertex2f(verts[0], verts[1]);

                // Vertex i
                rlTexCoord2f(uvs[i * 2], uvs[i * 2 + 1]);
                rlVertex2f(verts[i * 2], verts[i * 2 + 1]);

                // Vertex i+1
                rlTexCoord2f(uvs[(i + 1) * 2], uvs[(i + 1) * 2 + 1]);
                rlVertex2f(verts[(i + 1) * 2], verts[(i + 1) * 2 + 1]);

                // Degenerate 4th vert for RL_QUADS (repeat last)
                rlTexCoord2f(uvs[(i + 1) * 2], uvs[(i + 1) * 2 + 1]);
                rlVertex2f(verts[(i + 1) * 2], verts[(i + 1) * 2 + 1]);
            }
        rlEnd();
        rlSetTexture(0);
        return 0;
    }

    // DrawTexturePolyStrip(texture, vertices, texcoords, vertexCount, color)
    //   Draws as triangle strip (each 3 consecutive vertices form a triangle)
    static int native_DrawTexturePolyStrip(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        int vertCount = args[3].asInt();
        if (vertCount < 3) return 0;
        Color *tint = (Color *)args[4].asNativeStructInstance()->data;

        int vCount = 0, tCount = 0;
        std::vector<float> vScratch, tScratch;
        const float *verts = resolveFloat2Array(args[1], vCount, vScratch);
        const float *uvs = resolveFloat2Array(args[2], tCount, tScratch);
        if (!verts || !uvs) return 0;

        rlSetTexture(tex->id);
        rlBegin(RL_QUADS);
            rlColor4ub(tint->r, tint->g, tint->b, tint->a);

            for (int i = 0; i < vertCount - 2; i++)
            {
                int i0, i1, i2;
                if (i % 2 == 0) { i0 = i; i1 = i + 1; i2 = i + 2; }
                else            { i0 = i; i1 = i + 2; i2 = i + 1; }

                rlTexCoord2f(uvs[i0 * 2], uvs[i0 * 2 + 1]);
                rlVertex2f(verts[i0 * 2], verts[i0 * 2 + 1]);

                rlTexCoord2f(uvs[i1 * 2], uvs[i1 * 2 + 1]);
                rlVertex2f(verts[i1 * 2], verts[i1 * 2 + 1]);

                rlTexCoord2f(uvs[i2 * 2], uvs[i2 * 2 + 1]);
                rlVertex2f(verts[i2 * 2], verts[i2 * 2 + 1]);

                rlTexCoord2f(uvs[i2 * 2], uvs[i2 * 2 + 1]);
                rlVertex2f(verts[i2 * 2], verts[i2 * 2 + 1]);
            }
        rlEnd();
        rlSetTexture(0);
        return 0;
    }

    // GetTextureSize(texture) -> (width, height)
    static int native_GetTextureSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        vm->pushInt(tex->width);
        vm->pushInt(tex->height);
        return 2;
    }

    // GetTextureId(texture) -> int (OpenGL texture ID)
    static int native_GetTextureId(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        vm->pushInt((int)tex->id);
        return 1;
    }

    // DrawTextureTriangles(texture, vertices, texcoords, vertexCount, color)
    //   vertices/texcoords: flat float arrays [x0,y0, x1,y1, ...]
    //   Every 3 vertices = 1 triangle (triangle list, pre-triangulated)
    //   Accepts buffer (zero-copy) or BuLang array
    //   Use for complex/concave polygons like map contours
    static int native_DrawTextureTriangles(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        int vertCount = args[3].asInt();
        if (vertCount < 3) return 0;
        Color *tint = (Color *)args[4].asNativeStructInstance()->data;

        int vCount = 0, tCount = 0;
        std::vector<float> vScratch, tScratch;
        const float *verts = resolveFloat2Array(args[1], vCount, vScratch);
        const float *uvs = resolveFloat2Array(args[2], tCount, tScratch);
        if (!verts || !uvs) return 0;

        int triCount = vertCount / 3;
        rlSetTexture(tex->id);
        rlBegin(RL_QUADS);
            rlColor4ub(tint->r, tint->g, tint->b, tint->a);

            for (int t = 0; t < triCount; t++)
            {
                int base = t * 3;
                int i0 = base, i1 = base + 1, i2 = base + 2;

                rlTexCoord2f(uvs[i0 * 2], uvs[i0 * 2 + 1]);
                rlVertex2f(verts[i0 * 2], verts[i0 * 2 + 1]);

                rlTexCoord2f(uvs[i1 * 2], uvs[i1 * 2 + 1]);
                rlVertex2f(verts[i1 * 2], verts[i1 * 2 + 1]);

                rlTexCoord2f(uvs[i2 * 2], uvs[i2 * 2 + 1]);
                rlVertex2f(verts[i2 * 2], verts[i2 * 2 + 1]);

                // Degenerate 4th for RL_QUADS
                rlTexCoord2f(uvs[i2 * 2], uvs[i2 * 2 + 1]);
                rlVertex2f(verts[i2 * 2], verts[i2 * 2 + 1]);
            }
        rlEnd();
        rlSetTexture(0);
        return 0;
    }

    // DrawTextureTrianglesIndexed(texture, vertices, texcoords, indices, indexCount, color)
    //   vertices/texcoords: flat float arrays (vertex pool)
    //   indices: int array [i0,i1,i2, i3,i4,i5, ...] (every 3 = 1 triangle)
    //   Allows vertex reuse for complex meshes
    static int native_DrawTextureTrianglesIndexed(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6 || !args[0].isPointer()) return 0;
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        int idxCount = args[4].asInt();
        if (idxCount < 3) return 0;
        Color *tint = (Color *)args[5].asNativeStructInstance()->data;

        int vCount = 0, tCount = 0;
        std::vector<float> vScratch, tScratch;
        const float *verts = resolveFloat2Array(args[1], vCount, vScratch);
        const float *uvs = resolveFloat2Array(args[2], tCount, tScratch);
        if (!verts || !uvs) return 0;

        // Read indices
        std::vector<int> indices;
        if (args[3].isBuffer())
        {
            BufferInstance *buf = args[3].asBuffer();
            if (!buf) return 0;
            int count = (buf->count * buf->elementSize) / (int)sizeof(int);
            if (count < idxCount) return 0;
            const int *idata = (const int *)buf->data;
            indices.assign(idata, idata + idxCount);
        }
        else if (args[3].isArray())
        {
            ArrayInstance *arr = args[3].asArray();
            if (!arr || (int)arr->values.size() < idxCount) return 0;
            indices.resize(idxCount);
            for (int i = 0; i < idxCount; i++)
                indices[i] = arr->values[i].asInt();
        }
        else return 0;

        int triCount = idxCount / 3;
        rlSetTexture(tex->id);
        rlBegin(RL_QUADS);
            rlColor4ub(tint->r, tint->g, tint->b, tint->a);

            for (int t = 0; t < triCount; t++)
            {
                int i0 = indices[t * 3];
                int i1 = indices[t * 3 + 1];
                int i2 = indices[t * 3 + 2];

                rlTexCoord2f(uvs[i0 * 2], uvs[i0 * 2 + 1]);
                rlVertex2f(verts[i0 * 2], verts[i0 * 2 + 1]);

                rlTexCoord2f(uvs[i1 * 2], uvs[i1 * 2 + 1]);
                rlVertex2f(verts[i1 * 2], verts[i1 * 2 + 1]);

                rlTexCoord2f(uvs[i2 * 2], uvs[i2 * 2 + 1]);
                rlVertex2f(verts[i2 * 2], verts[i2 * 2 + 1]);

                rlTexCoord2f(uvs[i2 * 2], uvs[i2 * 2 + 1]);
                rlVertex2f(verts[i2 * 2], verts[i2 * 2 + 1]);
            }
        rlEnd();
        rlSetTexture(0);
        return 0;
    }

    // ========================================
    // REGISTER TEXTURES
    // ========================================

    void register_textures(Interpreter &vm)
    {
        vm.registerNative("LoadTexture", native_LoadTexture, 1);
        vm.registerNative("UnloadTexture", native_UnloadTexture, 1);
        vm.registerNative("IsTextureReady", native_IsTextureReady, 1);
        vm.registerNative("SetTextureFilter", native_SetTextureFilter, 2);
        vm.registerNative("SetTextureWrap", native_SetTextureWrap, 2);
        vm.registerNative("DrawTexture", native_DrawTexture, 4);
        vm.registerNative("DrawTextureRec", native_DrawTextureRec, 4);
        vm.registerNative("DrawTexturePro", native_DrawTexturePro, 6);
        vm.registerNative("DrawTextureV", native_DrawTextureV, 3);
        vm.registerNative("DrawTextureEx", native_DrawTextureEx, 6);
        vm.registerNative("LoadRenderTexture", native_LoadRenderTexture, 2);
        vm.registerNative("UnloadRenderTexture", native_UnloadRenderTexture, 1);
        vm.registerNative("BeginTextureMode", native_BeginTextureMode, 1);
        vm.registerNative("EndTextureMode", native_EndTextureMode, 0);
        vm.registerNative("GetRenderTextureTexture", native_GetRenderTextureTexture, 1);
            // Image
        vm.registerNative("LoadImage", native_LoadImage, 1);
        vm.registerNative("UnloadImage", native_UnloadImage, 1);
        vm.registerNative("LoadTextureFromImage", native_LoadTextureFromImage, 1);
        vm.registerNative("GenImageColor", native_GenImageColor, 3);
        vm.registerNative("GenImageChecked", native_GenImageChecked, 6);
        vm.registerNative("ImageFlipVertical", native_ImageFlipVertical, 1);
        vm.registerNative("ImageFlipHorizontal", native_ImageFlipHorizontal, 1);
        vm.registerNative("ImageResize", native_ImageResize, 3);
        vm.registerNative("ExportImage", native_ExportImage, 2);
        vm.registerNative("LoadImageFromScreen", native_LoadImageFromScreen, 0);
        vm.registerNative("IsImageReady", native_IsImageReady, 1);
        vm.registerNative("ImageCopy", native_ImageCopy, 1);
        vm.registerNative("ImageFromImage", native_ImageFromImage, 2);
        vm.registerNative("ImageCrop", native_ImageCrop, 2);
        vm.registerNative("ImageAlphaCrop", native_ImageAlphaCrop, 2);
        vm.registerNative("ImageAlphaClear", native_ImageAlphaClear, 3);
        vm.registerNative("ImageAlphaPremultiply", native_ImageAlphaPremultiply, 1);
        vm.registerNative("ImageBlurGaussian", native_ImageBlurGaussian, 2);
        vm.registerNative("ImageResizeNN", native_ImageResizeNN, 3);
        vm.registerNative("ImageResizeCanvas", native_ImageResizeCanvas, 6);
        vm.registerNative("ImageMipmaps", native_ImageMipmaps, 1);
        vm.registerNative("ImageFormat", native_ImageFormat, 2);
        vm.registerNative("ImageRotate", native_ImageRotate, 2);
        vm.registerNative("ImageRotateCW", native_ImageRotateCW, 1);
        vm.registerNative("ImageRotateCCW", native_ImageRotateCCW, 1);
            // Image color ops
        vm.registerNative("ImageColorTint", native_ImageColorTint, 2);
        vm.registerNative("ImageColorInvert", native_ImageColorInvert, 1);
        vm.registerNative("ImageColorGrayscale", native_ImageColorGrayscale, 1);
        vm.registerNative("ImageColorContrast", native_ImageColorContrast, 2);
        vm.registerNative("ImageColorBrightness", native_ImageColorBrightness, 2);
        vm.registerNative("ImageColorReplace", native_ImageColorReplace, 3);
        vm.registerNative("GetImageColor", native_GetImageColor, 3);
        vm.registerNative("GetImageAlphaBorder", native_GetImageAlphaBorder, 2);
        vm.registerNative("GetImageSize", native_GetImageSize, 1);
        vm.registerNative("LoadImageFromTexture", native_LoadImageFromTexture, 1);
            // Textured polygon drawing
        vm.registerNative("DrawTexturePoly", native_DrawTexturePoly, 5);
        vm.registerNative("DrawTexturePolyStrip", native_DrawTexturePolyStrip, 5);
        vm.registerNative("GetTextureSize", native_GetTextureSize, 1);
        vm.registerNative("GetTextureId", native_GetTextureId, 1);
        vm.registerNative("DrawTextureTriangles", native_DrawTextureTriangles, 5);
        vm.registerNative("DrawTextureTrianglesIndexed", native_DrawTextureTrianglesIndexed, 6);
            // Image generation
        vm.registerNative("GenImageGradientLinear", native_GenImageGradientLinear, 5);
        vm.registerNative("GenImageGradientRadial", native_GenImageGradientRadial, 5);
        vm.registerNative("GenImageGradientSquare", native_GenImageGradientSquare, 5);
        vm.registerNative("GenImageWhiteNoise", native_GenImageWhiteNoise, 3);
        vm.registerNative("GenImagePerlinNoise", native_GenImagePerlinNoise, 5);
        vm.registerNative("GenImageCellular", native_GenImageCellular, 3);
            // Image drawing
        vm.registerNative("ImageClearBackground", native_ImageClearBackground, 2);
        vm.registerNative("ImageDrawPixel", native_ImageDrawPixel, 4);
        vm.registerNative("ImageDrawLine", native_ImageDrawLine, 6);
        vm.registerNative("ImageDrawCircle", native_ImageDrawCircle, 5);
        vm.registerNative("ImageDrawCircleLines", native_ImageDrawCircleLines, 5);
        vm.registerNative("ImageDrawRectangle", native_ImageDrawRectangle, 6);
        vm.registerNative("ImageDrawRectangleRec", native_ImageDrawRectangleRec, 3);
        vm.registerNative("ImageDrawRectangleLines", native_ImageDrawRectangleLines, 4);
        vm.registerNative("ImageDraw", native_ImageDraw, 5);
        vm.registerNative("ImageDrawText", native_ImageDrawText, 6);
            // Shader
        vm.registerNative("LoadShader", native_LoadShader, 2);
        vm.registerNative("LoadShaderFromMemory", native_LoadShaderFromMemory, 2);
        vm.registerNative("UnloadShader", native_UnloadShader, 1);
        vm.registerNative("IsShaderReady", native_IsShaderReady, 1);
        vm.registerNative("GetShaderLocation", native_GetShaderLocation, 2);
        vm.registerNative("SetShaderValueFloat", native_SetShaderValueFloat, 3);
        vm.registerNative("SetShaderValueVec2", native_SetShaderValueVec2, 4);
        vm.registerNative("SetShaderValueVec3", native_SetShaderValueVec3, 5);
        vm.registerNative("SetShaderValueVec4", native_SetShaderValueVec4, 6);
        vm.registerNative("SetShaderValueInt", native_SetShaderValueInt, 3);
        vm.registerNative("BeginShaderMode", native_BeginShaderMode, 1);
        vm.registerNative("EndShaderMode", native_EndShaderMode, 0);
            // Color helpers
        vm.registerNative("ColorFromHSV", native_ColorFromHSV, 3);
        vm.registerNative("GetColor", native_GetColor, 1);
        vm.registerNative("ColorAlpha", native_ColorAlpha, 2);
        vm.registerNative("ColorToInt", native_ColorToInt, 1);
        vm.registerNative("ColorNormalize", native_ColorNormalize, 1);
        vm.registerNative("ColorToHSV", native_ColorToHSV, 1);
        vm.registerNative("ColorTint", native_ColorTint, 2);
        vm.registerNative("ColorBrightness", native_ColorBrightness, 2);
        vm.registerNative("ColorContrast", native_ColorContrast, 2);
        vm.registerNative("SetShaderValueTexture", native_SetShaderValueTexture, 3);
            // Misc
        vm.registerNative("TakeScreenshot", native_TakeScreenshot, 1);
        vm.registerNative("SetRandomSeed", native_SetRandomSeed, 1);
        vm.registerNative("GetRandomValue", native_GetRandomValue, 2);
            // Texture filter constants
        vm.addGlobal("TEXTURE_FILTER_POINT", vm.makeInt(TEXTURE_FILTER_POINT));
        vm.addGlobal("TEXTURE_FILTER_BILINEAR", vm.makeInt(TEXTURE_FILTER_BILINEAR));
        vm.addGlobal("TEXTURE_FILTER_TRILINEAR", vm.makeInt(TEXTURE_FILTER_TRILINEAR));
        vm.addGlobal("TEXTURE_FILTER_ANISOTROPIC_4X", vm.makeInt(TEXTURE_FILTER_ANISOTROPIC_4X));
        vm.addGlobal("TEXTURE_FILTER_ANISOTROPIC_8X", vm.makeInt(TEXTURE_FILTER_ANISOTROPIC_8X));
        vm.addGlobal("TEXTURE_FILTER_ANISOTROPIC_16X", vm.makeInt(TEXTURE_FILTER_ANISOTROPIC_16X));
            // Texture wrap constants
        vm.addGlobal("TEXTURE_WRAP_REPEAT", vm.makeInt(TEXTURE_WRAP_REPEAT));
        vm.addGlobal("TEXTURE_WRAP_CLAMP", vm.makeInt(TEXTURE_WRAP_CLAMP));
        vm.addGlobal("TEXTURE_WRAP_MIRROR_REPEAT", vm.makeInt(TEXTURE_WRAP_MIRROR_REPEAT));
        vm.addGlobal("TEXTURE_WRAP_MIRROR_CLAMP", vm.makeInt(TEXTURE_WRAP_MIRROR_CLAMP));
            // Pixel format constants (for ImageFormat)
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_GRAYSCALE", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_GRAYSCALE));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R5G6B5", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R5G6B5));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R8G8B8", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R8G8B8));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R5G5B5A1", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R4G4B4A4", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R4G4B4A4));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R8G8B8A8", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R8G8B8A8));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R32", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R32));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R32G32B32", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R32G32B32));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R32G32B32A32", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R32G32B32A32));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R16", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R16));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R16G16B16", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R16G16B16));
        vm.addGlobal("PIXELFORMAT_UNCOMPRESSED_R16G16B16A16", vm.makeInt(PIXELFORMAT_UNCOMPRESSED_R16G16B16A16));
    }

}
