// ================================================================
// sdkmesh_convert.cpp
// Compila: g++ -std=c++17 cn.cpp -o cn
// Uso OBJ: ./cn input.sdkmesh -o saida.obj
// Uso H3D: ./cn input.sdkmesh -h saida.h3d
// ================================================================

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <map>

#define SDKMESH_FILE_VERSION    101
#define SDKMESH_FILE_VERSION_V2 200
#define MAX_VERTEX_ELEMENTS     32
#define MAX_VERTEX_STREAMS      16
#define MAX_MESH_NAME           100
#define MAX_SUBSET_NAME         100
#define MAX_MATERIAL_NAME       100
#define MAX_TEXTURE_NAME        260
#define MAX_MATERIAL_PATH       260
#define MAX_FRAME_NAME          100

#define D3DDECLTYPE_FLOAT1    0
#define D3DDECLTYPE_FLOAT2    1
#define D3DDECLTYPE_FLOAT3    2
#define D3DDECLTYPE_FLOAT4    3
#define D3DDECLTYPE_UBYTE4N   8
#define D3DDECLTYPE_SHORT4N   10
#define D3DDECLTYPE_DEC3N     14
#define D3DDECLTYPE_FLOAT16_2 15
#define D3DDECLTYPE_FLOAT16_4 16
#define D3DDECLTYPE_UNUSED    17

#define D3DDECLUSAGE_POSITION  0
#define D3DDECLUSAGE_NORMAL    3
#define D3DDECLUSAGE_TEXCOORD  5
#define D3DDECLUSAGE_TANGENT   6
#define D3DDECLUSAGE_BINORMAL  7

#define PT_TRIANGLE_LIST  0
#define PT_TRIANGLE_STRIP 1
#define IT_16BIT 0
#define IT_32BIT 1

// ============================================================
// Estruturas SDKMesh
// ============================================================

#pragma pack(push, 4)
struct D3DVERTEXELEMENT9 {
    uint16_t Stream, Offset;
    uint8_t  Type, Method, Usage, UsageIndex;
};
#pragma pack(pop)

#pragma pack(push, 8)
struct SDKMESH_HEADER {
    uint32_t Version;
    uint8_t  IsBigEndian, _pad[3];
    uint64_t HeaderSize, NonBufferDataSize, BufferDataSize;
    uint32_t NumVertexBuffers, NumIndexBuffers, NumMeshes;
    uint32_t NumTotalSubsets, NumFrames, NumMaterials;
    uint64_t VertexStreamHeadersOffset, IndexStreamHeadersOffset;
    uint64_t MeshDataOffset, SubsetDataOffset;
    uint64_t FrameDataOffset, MaterialDataOffset;
};

struct SDKMESH_VERTEX_BUFFER_HEADER {
    uint64_t          NumVertices, SizeBytes, StrideBytes;
    D3DVERTEXELEMENT9 Decl[MAX_VERTEX_ELEMENTS];
    uint64_t          DataOffset;
};

struct SDKMESH_INDEX_BUFFER_HEADER {
    uint64_t NumIndices, SizeBytes;
    uint32_t IndexType, _pad;
    uint64_t DataOffset;
};

struct SDKMESH_MESH {
    char     Name[MAX_MESH_NAME];
    uint8_t  NumVertexBuffers, _pad[3];
    uint32_t VertexBuffers[MAX_VERTEX_STREAMS];
    uint32_t IndexBuffer, NumSubsets, NumFrameInfluences;
    float    BoundingBoxCenter[3], BoundingBoxExtents[3];
    uint8_t  _pad3[4];
    uint64_t SubsetOffset;        
    uint64_t FrameInfluenceOffset;
};

struct SDKMESH_SUBSET {
    char     Name[MAX_SUBSET_NAME];
    uint32_t MaterialID, PrimitiveType;
    uint64_t IndexStart, IndexCount, VertexStart, VertexCount;
};

struct SDKMESH_MATERIAL {
    char    Name[MAX_MATERIAL_NAME];
    char    MaterialInstancePath[MAX_MATERIAL_PATH];
    char    DiffuseTexture[MAX_TEXTURE_NAME];
    char    NormalTexture[MAX_TEXTURE_NAME];
    char    SpecularTexture[MAX_TEXTURE_NAME];
    float   Diffuse[4], Ambient[4], Specular[4], Emissive[4], Power;
    uint64_t Force64_1,Force64_2,Force64_3,Force64_4,Force64_5,Force64_6;
};

struct SDKMESH_MATERIAL_V2 {
    char    Name[MAX_MATERIAL_NAME];
    char    RMATexture[MAX_TEXTURE_NAME];
    char    AlbedoTexture[MAX_TEXTURE_NAME];
    char    NormalTexture[MAX_TEXTURE_NAME];
    char    EmissiveTexture[MAX_TEXTURE_NAME];
    float   Alpha;
    char    Reserved[60];
    uint64_t Force64_1,Force64_2,Force64_3,Force64_4,Force64_5,Force64_6;
};
#pragma pack(pop)

// ============================================================
// Utilitários
// ============================================================

static std::string safeStr(const char* p, size_t n) { return std::string(p, strnlen(p, n)); }

static std::string fileNoExt(const std::string& s) {
    std::string r = s;
    auto a = r.rfind('\\'); if (a != std::string::npos) r = r.substr(a+1);
    auto b = r.rfind('/');  if (b != std::string::npos) r = r.substr(b+1);
    auto c = r.rfind('.');  if (c != std::string::npos) r = r.substr(0,c);
    return r;
}

static std::string fileOnly(const std::string& s) {
    std::string r = s;
    auto a = r.rfind('\\'); if (a != std::string::npos) r = r.substr(a+1);
    auto b = r.rfind('/');  if (b != std::string::npos) r = r.substr(b+1);
    return r;
}

static bool inBounds(long fs, uint64_t offset, uint64_t len)
{ return len <= (uint64_t)fs && offset <= (uint64_t)fs - len; }

static float rf(const uint8_t* p, int o) { float v; memcpy(&v,p+o,4); return v; }

static float h2f(uint16_t h) {
    uint32_t s=(h>>15)&1, e=(h>>10)&0x1f, m=h&0x3ff, f;
    if (e==0)  { if (!m) f=s<<31; else { while(!(m&0x400)){m<<=1;e--;} e++;m&=~0x400u; f=(s<<31)|((e+112)<<23)|(m<<13); } }
    else if (e==31) f=(s<<31)|0x7f800000u|(m<<13);
    else            f=(s<<31)|((e+112)<<23)|(m<<13);
    float r; memcpy(&r,&f,4); return r;
}

static void rv3(const uint8_t* p, int o, int t, float& x, float& y, float& z) {
    x=y=z=0;
    if (t==D3DDECLTYPE_FLOAT3||t==D3DDECLTYPE_FLOAT4) { x=rf(p,o);y=rf(p,o+4);z=rf(p,o+8); }
    else if (t==D3DDECLTYPE_FLOAT16_4) { uint16_t a,b,c; memcpy(&a,p+o,2);memcpy(&b,p+o+2,2);memcpy(&c,p+o+4,2); x=h2f(a);y=h2f(b);z=h2f(c); }
    else if (t==D3DDECLTYPE_SHORT4N) { int16_t a,b,c; memcpy(&a,p+o,2);memcpy(&b,p+o+2,2);memcpy(&c,p+o+4,2); x=a/32767.f;y=b/32767.f;z=c/32767.f; }
}

static void rv2(const uint8_t* p, int o, int t, float& u, float& v) {
    u=v=0;
    if (t==D3DDECLTYPE_FLOAT2||t==D3DDECLTYPE_FLOAT3) { u=rf(p,o);v=rf(p,o+4); }
    else if (t==D3DDECLTYPE_FLOAT16_2) { uint16_t a,b; memcpy(&a,p+o,2);memcpy(&b,p+o+2,2); u=h2f(a);v=h2f(b); }
}

struct Attrib {
    int posOfs=-1, posT=D3DDECLTYPE_FLOAT3;
    int nrmOfs=-1, nrmT=D3DDECLTYPE_FLOAT3;
    int texOfs=-1, texT=D3DDECLTYPE_FLOAT2;
};

static Attrib decodeDecl(const D3DVERTEXELEMENT9* d, int stride) {
    Attrib a;
    for (int i=0; i<MAX_VERTEX_ELEMENTS; i++) {
        if (d[i].Type==D3DDECLTYPE_UNUSED) break;
        if (d[i].UsageIndex==0) {
            if (d[i].Usage==D3DDECLUSAGE_POSITION) { a.posOfs=d[i].Offset; a.posT=d[i].Type; }
            if (d[i].Usage==D3DDECLUSAGE_NORMAL)   { a.nrmOfs=d[i].Offset; a.nrmT=d[i].Type; }
            if (d[i].Usage==D3DDECLUSAGE_TEXCOORD) { a.texOfs=d[i].Offset; a.texT=d[i].Type; }
        }
    }
    return a;
}

// ============================================================
// Dados em memória
// ============================================================

struct Vert { float x,y,z,nx,ny,nz,u,v; };

struct SubMesh {
    std::string name, matId, diffTex;
    std::vector<uint32_t> idx;
};

struct Mesh {
    std::string name;
    uint32_t    vbIdx;
    std::vector<Vert>    verts;
    std::vector<SubMesh> subs;
};

// ============================================================
// Loader
// ============================================================

bool loadSDKMesh(const char* path, std::vector<Mesh>& out)
{
    FILE* f = fopen(path,"rb");
    if (!f) { printf("Cannot open: %s\n",path); return false; }
    fseek(f,0,SEEK_END); long fsz=ftell(f); fseek(f,0,SEEK_SET);
    std::vector<uint8_t> data(fsz);
    if ((long)fread(data.data(),1,fsz,f)!=fsz) { fclose(f); return false; }
    fclose(f);

    const uint8_t* B = data.data();
    if (fsz < (long)sizeof(SDKMESH_HEADER)) { printf("Too small\n"); return false; }

    const SDKMESH_HEADER* H = (const SDKMESH_HEADER*)B;
    if (H->Version!=SDKMESH_FILE_VERSION && H->Version!=SDKMESH_FILE_VERSION_V2)
    { printf("Bad version %u\n",H->Version); return false; }

    const bool v2 = (H->Version==SDKMESH_FILE_VERSION_V2);
    printf("[LOADER] Loading SDKMesh v%u (%u meshes, %u materials)...\n", H->Version, H->NumMeshes, H->NumMaterials);

    const auto* VBH = (const SDKMESH_VERTEX_BUFFER_HEADER*)(B + H->VertexStreamHeadersOffset);
    const auto* IBH = (const SDKMESH_INDEX_BUFFER_HEADER*) (B + H->IndexStreamHeadersOffset);
    const auto* MSH = (const SDKMESH_MESH*)                (B + H->MeshDataOffset);
    const SDKMESH_SUBSET* AS = (const SDKMESH_SUBSET*)     (B + H->SubsetDataOffset);

    struct MI { std::string diff; };
    std::vector<MI> mats(H->NumMaterials);
    for (uint32_t i=0; i<H->NumMaterials; i++) {
        uint64_t mo = H->MaterialDataOffset + (uint64_t)i * sizeof(SDKMESH_MATERIAL);
        if (!inBounds(fsz, mo, sizeof(SDKMESH_MATERIAL))) break;
        if (v2) mats[i].diff = safeStr(((const SDKMESH_MATERIAL_V2*)(B+mo))->AlbedoTexture, MAX_TEXTURE_NAME);
        else    mats[i].diff = safeStr(((const SDKMESH_MATERIAL*)   (B+mo))->DiffuseTexture, MAX_TEXTURE_NAME);
    }

    for (uint32_t mi=0; mi<H->NumMeshes; mi++)
    {
        const SDKMESH_MESH& M = MSH[mi];
        std::string mname = safeStr(M.Name, MAX_MESH_NAME);

        uint32_t vbIdx = M.VertexBuffers[0];
        if (vbIdx >= H->NumVertexBuffers) continue;
        const SDKMESH_VERTEX_BUFFER_HEADER& VB = VBH[vbIdx];
        const int stride = (int)VB.StrideBytes;
        const uint64_t vbAbs = VB.DataOffset;
        if (!inBounds(fsz, vbAbs, VB.NumVertices*(uint64_t)stride)) continue;

        uint32_t ibIdx = M.IndexBuffer;
        if (ibIdx >= H->NumIndexBuffers) continue;
        const SDKMESH_INDEX_BUFFER_HEADER& IB = IBH[ibIdx];
        const uint64_t ibAbs = IB.DataOffset;
        if (!inBounds(fsz, ibAbs, IB.SizeBytes)) continue;

        const uint64_t siAbs = M.SubsetOffset;
        if (!inBounds(fsz, siAbs, (uint64_t)M.NumSubsets * sizeof(int32_t))) continue;

        Attrib layout = decodeDecl(VB.Decl, stride);
        if (layout.posOfs < 0) layout.posOfs = 0;

        const uint8_t* vbd = B + vbAbs;
        const uint8_t* ibd = B + ibAbs;
        const bool i32 = (IB.IndexType==IT_32BIT);

        auto rIdx = [&](uint64_t p) -> uint32_t {
            if (i32) { uint32_t v; memcpy(&v,ibd+p*4,4); return v; }
            else     { uint16_t v; memcpy(&v,ibd+p*2,2); return (uint32_t)v; }
        };

        Mesh mesh;
        mesh.name=mname; mesh.vbIdx=vbIdx;
        mesh.verts.reserve((size_t)VB.NumVertices);
        for (uint64_t vi=0; vi<VB.NumVertices; vi++) {
            const uint8_t* vp = vbd + vi*stride;
            Vert vt={};
            rv3(vp, layout.posOfs, layout.posT, vt.x,vt.y,vt.z);
            if (layout.nrmOfs>=0) rv3(vp, layout.nrmOfs, layout.nrmT, vt.nx,vt.ny,vt.nz);
            if (layout.texOfs>=0) rv2(vp, layout.texOfs, layout.texT, vt.u,vt.v);
            mesh.verts.push_back(vt);
        }

        const int32_t* SI = (const int32_t*)(B + siAbs);
        for (uint32_t si=0; si<M.NumSubsets; si++) {
            int32_t gIdx = SI[si];
            if (gIdx<0 || (uint32_t)gIdx>=H->NumTotalSubsets) continue;

            const SDKMESH_SUBSET& SS = AS[(uint32_t)gIdx];
            if (SS.PrimitiveType!=PT_TRIANGLE_LIST && SS.PrimitiveType!=PT_TRIANGLE_STRIP) continue;
            if (SS.IndexStart+SS.IndexCount > IB.NumIndices) continue;
            if (SS.VertexStart+SS.VertexCount > VB.NumVertices) continue;

            SubMesh sm;
            sm.name = safeStr(SS.Name, MAX_SUBSET_NAME);
            
            // Garantir que todas as meshes têm um material associado
            if (SS.MaterialID < H->NumMaterials && !mats[SS.MaterialID].diff.empty()) {
                sm.diffTex = fileOnly(mats[SS.MaterialID].diff);
                sm.matId   = std::to_string(SS.MaterialID); // Usar ID como string
            } else {
                sm.diffTex = "default.png";
                sm.matId   = "0";
            }

            if (SS.PrimitiveType==PT_TRIANGLE_LIST) {
                sm.idx.reserve((size_t)SS.IndexCount);
                for (uint64_t ii=0; ii+2<SS.IndexCount; ii+=3)
                    for (int k=0; k<3; k++)
                        sm.idx.push_back(rIdx(SS.IndexStart+ii+k) + (uint32_t)SS.VertexStart);
            } else {
                for (uint64_t ii=0; ii+2<SS.IndexCount; ii++) {
                    uint32_t i0=rIdx(SS.IndexStart+ii), i1=rIdx(SS.IndexStart+ii+1), i2=rIdx(SS.IndexStart+ii+2);
                    if (i0==i1||i1==i2||i0==i2) continue;
                    if (ii&1) std::swap(i0,i1);
                    sm.idx.push_back(i0+(uint32_t)SS.VertexStart);
                    sm.idx.push_back(i1+(uint32_t)SS.VertexStart);
                    sm.idx.push_back(i2+(uint32_t)SS.VertexStart);
                }
            }
            if (!sm.idx.empty()) mesh.subs.push_back(std::move(sm));
        }
        out.push_back(std::move(mesh));
    }
    return !out.empty();
}

// ============================================================
// Export OBJ Format
// ============================================================

bool exportOBJ(const std::vector<Mesh>& meshes, const char* path)
{
    FILE* f = fopen(path,"w");
    if (!f) { printf("[ERROR] Cannot write: %s\n",path); return false; }

    std::string mp = path;
    { auto d=mp.rfind('.'); if(d!=std::string::npos) mp=mp.substr(0,d); mp+=".mtl"; }
    auto sl=mp.rfind('/');
    std::string mb=(sl!=std::string::npos)?mp.substr(sl+1):mp;

    FILE* fm = fopen(mp.c_str(),"w");
    fprintf(f,"# SDKMesh -> OBJ\nmtllib %s\n\n",mb.c_str());

    std::unordered_map<uint32_t,uint32_t> vbOfs;
    uint32_t nextOfs=1;
    for (const Mesh& m : meshes) {
        if (vbOfs.count(m.vbIdx)) continue;
        vbOfs[m.vbIdx]=nextOfs;
        for (const Vert& v:m.verts) fprintf(f,"v  %.6f %.6f %.6f\n",v.x,v.y,v.z);
        for (const Vert& v:m.verts) fprintf(f,"vn %.6f %.6f %.6f\n",v.nx,v.ny,v.nz);
        for (const Vert& v:m.verts) fprintf(f,"vt %.6f %.6f\n",v.u,1.f-v.v);
        nextOfs+=(uint32_t)m.verts.size();
    }
    fprintf(f,"\n");

    std::unordered_map<std::string,bool> wmat;
    for (const Mesh& m : meshes) {
        uint32_t vo = vbOfs[m.vbIdx];
        fprintf(f,"o %s\n",m.name.c_str());
        for (const SubMesh& sm : m.subs) {
            if (!sm.matId.empty()) {
                if (fm && !wmat.count(sm.matId))
                { fprintf(fm,"newmtl %s\n  map_Kd %s\n\n",sm.matId.c_str(),sm.diffTex.c_str()); wmat[sm.matId]=true; }
                fprintf(f,"usemtl %s\n",sm.matId.c_str());
            }
            fprintf(f,"g %s\n",sm.name.c_str());
            for (size_t i=0; i+2<sm.idx.size(); i+=3) {
                uint32_t a=sm.idx[i]+vo, b=sm.idx[i+1]+vo, c=sm.idx[i+2]+vo;
                fprintf(f,"f %u/%u/%u %u/%u/%u %u/%u/%u\n",a,a,a,b,b,b,c,c,c);
            }
        }
        fprintf(f,"\n");
    }

    if (fm) fclose(fm);
    fclose(f);
    printf("[EXPORT] OBJ saved to: %s\n", path);
    return true;
}

// ============================================================
// Export Custom Format (.h3d) - Agrupado por Material
// ============================================================

struct CustomBuffer {
    std::string diffuseTex;
    std::vector<Vert> vertices;
    std::vector<uint32_t> indices;
};

// Helpers Binários
static void wU32(FILE* f, uint32_t v) { fwrite(&v, 4, 1, f); }
static void wF32(FILE* f, float v)    { fwrite(&v, 4, 1, f); }
static void wU8(FILE* f, uint8_t v)   { fwrite(&v, 1, 1, f); }
static void wStr(FILE* f, const std::string& s) { fwrite(s.c_str(), 1, s.length() + 1, f); }

static long beginChunk(FILE* f, uint32_t id) {
    wU32(f, id); wU32(f, 0); return ftell(f);
}

static void endChunk(FILE* f, long startPos) {
    long currentPos = ftell(f);
    uint32_t length = (uint32_t)(currentPos - startPos);
    fseek(f, startPos - 4, SEEK_SET); wU32(f, length); fseek(f, currentPos, SEEK_SET);
}

bool exportH3D(const std::vector<Mesh>& meshes, const char* path)
{
    FILE* f = fopen(path, "wb");
    if (!f) { printf("[ERROR] Cannot write: %s\n", path); return false; }

    // 1. Agrupar Geometria por Textura (Batching)
    std::map<std::string, CustomBuffer> groupedBuffers;

    for (const Mesh& m : meshes) {
        for (const SubMesh& sm : m.subs) {
            std::string texName = sm.diffTex;
            CustomBuffer& cb = groupedBuffers[texName];
            cb.diffuseTex = texName;

            uint32_t vertexOffset = (uint32_t)cb.vertices.size();

            // Desdobrar vértices e re-indexar
            for(uint32_t idx : sm.idx) {
                cb.vertices.push_back(m.verts[idx]);
                cb.indices.push_back((uint32_t)(cb.indices.size())); 
            }
        }
    }

    // 2. Escrever Cabeçalho H3D/MESH
    wU32(f, 0x4D455348); // "MESH" Magic
    wU32(f, 100);        // Versão 1.00

    // 3. Escrever Materiais (CHUNK_MATS)
    long matStart = beginChunk(f, 0x4D415453); // "MATS"
    wU32(f, (uint32_t)groupedBuffers.size()); 
    
    int materialIndex = 0;
    std::map<std::string, int> matIndexMap;

    for (auto& pair : groupedBuffers) {
        matIndexMap[pair.first] = materialIndex++;
        
        wStr(f, fileNoExt(pair.first)); // Nome do material
        
        // Cores (Diffuse, Specular, Shininess)
        wF32(f, 1.0f); wF32(f, 1.0f); wF32(f, 1.0f); 
        wF32(f, 0.0f); wF32(f, 0.0f); wF32(f, 0.0f); 
        wF32(f, 32.0f); 
        
        // Texturas (1 Layer)
        wU8(f, 1); 
        wStr(f, pair.first); 
    }
    endChunk(f, matStart);

    // 4. Escrever Buffers (CHUNK_BUFF)
    int bufferCount = 0;
    for (auto& pair : groupedBuffers) {
        CustomBuffer& cb = pair.second;
        if (cb.vertices.empty()) continue;

        long buffStart = beginChunk(f, 0x42554646); // "BUFF"

        wU32(f, matIndexMap[pair.first]); // Material Index
        wU32(f, 0); // Flags (0 = Estático)

        // Vértices (CHUNK_VRTS)
        long vrtsStart = beginChunk(f, 0x56525453); // "VRTS"
        wU32(f, (uint32_t)cb.vertices.size());
        for (const Vert& v : cb.vertices) {
            wF32(f, v.x); wF32(f, v.y); wF32(f, v.z);
            wF32(f, v.nx); wF32(f, v.ny); wF32(f, v.nz);
            wF32(f, v.u); wF32(f, v.v);
        }
        endChunk(f, vrtsStart);

        // Índices (CHUNK_IDXS)
        long idxsStart = beginChunk(f, 0x49445853); // "IDXS"
        wU32(f, (uint32_t)cb.indices.size());
        for (uint32_t i : cb.indices) { wU32(f, i); }
        endChunk(f, idxsStart);

        endChunk(f, buffStart);
        bufferCount++;
    }

    fclose(f);
    printf("[EXPORT] H3D Batch saved to: %s (Agrupado em %d Buffers Otimizados)\n", path, bufferCount);
    return true;
}

// ============================================================
// Main (Command Line Interface)
// ============================================================

void printUsage() {
    printf("--------------------------------------------------\n");
    printf(" BuGL SDKMesh Converter Tool\n");
    printf("--------------------------------------------------\n");
    printf(" Uso para formato Wavefront OBJ:\n");
    printf("   ./cn <modelo.sdkmesh> -o <saida.obj>\n\n");
    printf(" Uso para formato Binário H3D (Otimizado):\n");
    printf("   ./cn <modelo.sdkmesh> -h <saida.h3d>\n");
    printf("--------------------------------------------------\n");
}

int main(int argc, char* argv[])
{
    if (argc < 4) { 
        printUsage(); 
        return 1; 
    }

    std::string inputFile = argv[1];
    std::string mode      = argv[2];
    std::string outputFile = argv[3];

    std::vector<Mesh> meshes;
    if (!loadSDKMesh(inputFile.c_str(), meshes)) {
        return 1;
    }

    size_t tv=0, tt=0;
    for (const Mesh& m:meshes) { 
        tv+=m.verts.size(); 
        for(const SubMesh& s:m.subs) tt+=s.idx.size()/3; 
    }
    printf("[INFO] Extraido: %zu vertices, %zu triangulos de %zu sub-meshes\n", tv, tt, meshes.size());

    if (mode == "-o") {
        return exportOBJ(meshes, outputFile.c_str()) ? 0 : 1;
    } 
    else if (mode == "-h") {
        return exportH3D(meshes, outputFile.c_str()) ? 0 : 1;
    } 
    else {
        printf("[ERROR] Modo invalido: %s\n", mode.c_str());
        printUsage();
        return 1;
    }
}
