// ================================================================
// sdkmesh_convert.cpp
// Compila: g++ -std=c++17 sdkmesh_convert.cpp -o sdkmesh_convert
// Uso:     ./sdkmesh_convert input.sdkmesh output.obj
// ================================================================

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

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
// Estruturas — tamanhos verificados pelos static_assert Microsoft
// ============================================================

#pragma pack(push, 4)
struct D3DVERTEXELEMENT9
{
    uint16_t Stream, Offset;
    uint8_t  Type, Method, Usage, UsageIndex;
};
#pragma pack(pop)
static_assert(sizeof(D3DVERTEXELEMENT9) == 8, "");

#pragma pack(push, 8)

struct SDKMESH_HEADER
{
    uint32_t Version;
    uint8_t  IsBigEndian, _pad[3];
    uint64_t HeaderSize, NonBufferDataSize, BufferDataSize;
    uint32_t NumVertexBuffers, NumIndexBuffers, NumMeshes;
    uint32_t NumTotalSubsets, NumFrames, NumMaterials;
    uint64_t VertexStreamHeadersOffset, IndexStreamHeadersOffset;
    uint64_t MeshDataOffset, SubsetDataOffset;
    uint64_t FrameDataOffset, MaterialDataOffset;
};
static_assert(sizeof(SDKMESH_HEADER) == 104, "");

struct SDKMESH_VERTEX_BUFFER_HEADER
{
    uint64_t          NumVertices, SizeBytes, StrideBytes;
    D3DVERTEXELEMENT9 Decl[MAX_VERTEX_ELEMENTS];
    uint64_t          DataOffset;
};
static_assert(sizeof(SDKMESH_VERTEX_BUFFER_HEADER) == 288, "");

struct SDKMESH_INDEX_BUFFER_HEADER
{
    uint64_t NumIndices, SizeBytes;
    uint32_t IndexType, _pad;
    uint64_t DataOffset;
};
static_assert(sizeof(SDKMESH_INDEX_BUFFER_HEADER) == 32, "");

struct SDKMESH_MESH
{
    char     Name[MAX_MESH_NAME];
    uint8_t  NumVertexBuffers, _pad[3];
    uint32_t VertexBuffers[MAX_VERTEX_STREAMS];
    uint32_t IndexBuffer, NumSubsets, NumFrameInfluences;
    float    BoundingBoxCenter[3], BoundingBoxExtents[3];
    uint8_t  _pad3[4];
    uint64_t SubsetOffset;        // relativo a HeaderSize (NonBufferData)
    uint64_t FrameInfluenceOffset;
};
static_assert(sizeof(SDKMESH_MESH) == 224, "");

struct SDKMESH_SUBSET
{
    char     Name[MAX_SUBSET_NAME];
    uint32_t MaterialID, PrimitiveType;
    // 4 bytes padding implícito (pack(8) + uint64 alignment)
    uint64_t IndexStart, IndexCount, VertexStart, VertexCount;
};
static_assert(sizeof(SDKMESH_SUBSET) == 144, "");

struct SDKMESH_FRAME
{
    char     Name[MAX_FRAME_NAME];
    uint32_t Mesh, ParentFrame, ChildFrame, SiblingFrame;
    float    Matrix[16];
    uint32_t AnimationDataIndex;
};
static_assert(sizeof(SDKMESH_FRAME) == 184, "");

struct SDKMESH_MATERIAL  // v101 Phong
{
    char    Name[MAX_MATERIAL_NAME];
    char    MaterialInstancePath[MAX_MATERIAL_PATH];
    char    DiffuseTexture[MAX_TEXTURE_NAME];
    char    NormalTexture[MAX_TEXTURE_NAME];
    char    SpecularTexture[MAX_TEXTURE_NAME];
    float   Diffuse[4], Ambient[4], Specular[4], Emissive[4], Power;
    uint64_t Force64_1, Force64_2, Force64_3, Force64_4, Force64_5, Force64_6;
};
static_assert(sizeof(SDKMESH_MATERIAL) == 1256, "");

struct SDKMESH_MATERIAL_V2  // v200 PBR
{
    char    Name[MAX_MATERIAL_NAME];
    char    RMATexture[MAX_TEXTURE_NAME];
    char    AlbedoTexture[MAX_TEXTURE_NAME];
    char    NormalTexture[MAX_TEXTURE_NAME];
    char    EmissiveTexture[MAX_TEXTURE_NAME];
    float   Alpha;
    char    Reserved[60];
    uint64_t Force64_1, Force64_2, Force64_3, Force64_4, Force64_5, Force64_6;
};
static_assert(sizeof(SDKMESH_MATERIAL_V2) == sizeof(SDKMESH_MATERIAL), "");

#pragma pack(pop)

// ============================================================
// Utilitários
// ============================================================

static std::string safeStr(const char* p, size_t n)
{
    return std::string(p, strnlen(p, n));
}

static std::string fileNoExt(const std::string& s)
{
    std::string r = s;
    auto a = r.rfind('\\'); if (a != std::string::npos) r = r.substr(a+1);
    auto b = r.rfind('/');  if (b != std::string::npos) r = r.substr(b+1);
    auto c = r.rfind('.');  if (c != std::string::npos) r = r.substr(0,c);
    return r;
}

static std::string fileOnly(const std::string& s)
{
    std::string r = s;
    auto a = r.rfind('\\'); if (a != std::string::npos) r = r.substr(a+1);
    auto b = r.rfind('/');  if (b != std::string::npos) r = r.substr(b+1);
    return r;
}

static bool inBounds(long fs, uint64_t ofs, uint64_t len)
{
    return ofs + len <= (uint64_t)fs;
}

// ============================================================
// Leitura de vertex types
// ============================================================

static float rf(const uint8_t* p, int o)
{
    float v; memcpy(&v, p+o, 4); return v;
}

static float h2f(uint16_t h)
{
    uint32_t s=(h>>15)&1, e=(h>>10)&0x1f, m=h&0x3ff, f;
    if      (e==0)  f = m ? (e=0, [&]{ while(!(m&0x400)){m<<=1;e--;} e++;m&=~0x400u;
                             return (s<<31)|((e+112)<<23)|(m<<13); }()) : s<<31;
    else if (e==31) f = (s<<31)|0x7f800000u|(m<<13);
    else            f = (s<<31)|((e+112)<<23)|(m<<13);
    float r; memcpy(&r,&f,4); return r;
}

static void rv3(const uint8_t* p,int o,int t, float& x,float& y,float& z)
{
    x=y=z=0;
    if(t==D3DDECLTYPE_FLOAT3||t==D3DDECLTYPE_FLOAT4){ x=rf(p,o); y=rf(p,o+4); z=rf(p,o+8); }
    else if(t==D3DDECLTYPE_FLOAT16_4){ uint16_t a,b,c; memcpy(&a,p+o,2);memcpy(&b,p+o+2,2);memcpy(&c,p+o+4,2); x=h2f(a);y=h2f(b);z=h2f(c); }
    else if(t==D3DDECLTYPE_DEC3N){ uint32_t v; memcpy(&v,p+o,4); int32_t ix=(v>>0)&0x3ff; if(ix>=512)ix-=1024; int32_t iy=(v>>10)&0x3ff; if(iy>=512)iy-=1024; int32_t iz=(v>>20)&0x3ff; if(iz>=512)iz-=1024; x=ix/511.f;y=iy/511.f;z=iz/511.f; }
    else if(t==D3DDECLTYPE_SHORT4N){ int16_t a,b,c; memcpy(&a,p+o,2);memcpy(&b,p+o+2,2);memcpy(&c,p+o+4,2); x=a/32767.f;y=b/32767.f;z=c/32767.f; }
}

static void rv2(const uint8_t* p,int o,int t, float& u,float& v)
{
    u=v=0;
    if(t==D3DDECLTYPE_FLOAT2||t==D3DDECLTYPE_FLOAT3){ u=rf(p,o); v=rf(p,o+4); }
    else if(t==D3DDECLTYPE_FLOAT16_2){ uint16_t a,b; memcpy(&a,p+o,2);memcpy(&b,p+o+2,2); u=h2f(a);v=h2f(b); }
}

// ============================================================
// Vertex layout
// ============================================================

struct Attrib
{
    int posOfs=-1, posT=D3DDECLTYPE_FLOAT3;
    int nrmOfs=-1, nrmT=D3DDECLTYPE_FLOAT3;
    int texOfs=-1, texT=D3DDECLTYPE_FLOAT2;
};

static Attrib decodeDecl(const D3DVERTEXELEMENT9* d, int stride)
{
    Attrib a;
    printf("    layout stride=%d\n", stride);
    for(int i=0;i<MAX_VERTEX_ELEMENTS;i++)
    {
        if(d[i].Type==D3DDECLTYPE_UNUSED) break;
        if(d[i].UsageIndex==0)
        {
            if(d[i].Usage==D3DDECLUSAGE_POSITION){ a.posOfs=d[i].Offset; a.posT=d[i].Type; }
            if(d[i].Usage==D3DDECLUSAGE_NORMAL  ){ a.nrmOfs=d[i].Offset; a.nrmT=d[i].Type; }
            if(d[i].Usage==D3DDECLUSAGE_TEXCOORD){ a.texOfs=d[i].Offset; a.texT=d[i].Type; }
        }
    }
    return a;
}

// ============================================================
// Dados em memória
// ============================================================

struct Vert { float x,y,z,nx,ny,nz,u,v; };

struct SubMesh
{
    std::string name, matId, diffTex;
    std::vector<uint32_t> idx;
};

struct Mesh
{
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
    FILE* f=fopen(path,"rb");
    if(!f){ fprintf(stderr,"Cannot open: %s\n",path); return false; }
    fseek(f,0,SEEK_END); long fsz=ftell(f); fseek(f,0,SEEK_SET);
    std::vector<uint8_t> data(fsz);
    if((long)fread(data.data(),1,fsz,f)!=fsz){ fclose(f); return false; }
    fclose(f);

    const uint8_t* B=data.data();
    if(fsz<(long)sizeof(SDKMESH_HEADER)){ fprintf(stderr,"Too small\n"); return false; }

    const SDKMESH_HEADER* H=(const SDKMESH_HEADER*)B;
    if(H->Version!=SDKMESH_FILE_VERSION && H->Version!=SDKMESH_FILE_VERSION_V2)
    { fprintf(stderr,"Bad version %u\n",H->Version); return false; }
    if(H->IsBigEndian){ fprintf(stderr,"Big-endian not supported\n"); return false; }

    const bool v2=(H->Version==SDKMESH_FILE_VERSION_V2);
    printf("SDKMesh v%u  meshes=%u VBs=%u IBs=%u subsets=%u mats=%u\n",
        H->Version,H->NumMeshes,H->NumVertexBuffers,
        H->NumIndexBuffers,H->NumTotalSubsets,H->NumMaterials);

    const auto* VBH=(const SDKMESH_VERTEX_BUFFER_HEADER*)(B+H->VertexStreamHeadersOffset);
    const auto* IBH=(const SDKMESH_INDEX_BUFFER_HEADER*) (B+H->IndexStreamHeadersOffset);
    const auto* MSH=(const SDKMESH_MESH*)                (B+H->MeshDataOffset);

    // Array global de SDKMESH_SUBSET (structs reais — na zona de headers)
    const SDKMESH_SUBSET* AS=(const SDKMESH_SUBSET*)(B+H->SubsetDataOffset);

    // Materiais
    struct MI{ std::string diff; };
    std::vector<MI> mats(H->NumMaterials);
    for(uint32_t i=0;i<H->NumMaterials;i++)
    {
        uint64_t mo=H->MaterialDataOffset+(uint64_t)i*sizeof(SDKMESH_MATERIAL);
        if(!inBounds(fsz,mo,sizeof(SDKMESH_MATERIAL))) break;
        if(v2) mats[i].diff=safeStr(((const SDKMESH_MATERIAL_V2*)(B+mo))->AlbedoTexture,MAX_TEXTURE_NAME);
        else   mats[i].diff=safeStr(((const SDKMESH_MATERIAL*)   (B+mo))->DiffuseTexture,MAX_TEXTURE_NAME);
    }

    // bufBase = início dos dados VB/IB
    const uint64_t bufBase=H->HeaderSize+H->NonBufferDataSize;

    for(uint32_t mi=0;mi<H->NumMeshes;mi++)
    {
        const SDKMESH_MESH& M=MSH[mi];
        std::string mname=safeStr(M.Name,MAX_MESH_NAME);
        printf("Mesh[%u] '%s'\n",mi,mname.c_str());

        // VB
        uint32_t vbIdx=M.VertexBuffers[0];
        if(vbIdx>=H->NumVertexBuffers){ fprintf(stderr,"  bad VB\n"); continue; }
        const SDKMESH_VERTEX_BUFFER_HEADER& VB=VBH[vbIdx];
        const uint64_t vbAbs=bufBase+VB.DataOffset;
        if(!inBounds(fsz,vbAbs,VB.NumVertices*(uint64_t)VB.StrideBytes))
        { fprintf(stderr,"  VB OOB\n"); continue; }

        // IB
        uint32_t ibIdx=M.IndexBuffer;
        if(ibIdx>=H->NumIndexBuffers){ fprintf(stderr,"  bad IB\n"); continue; }
        const SDKMESH_INDEX_BUFFER_HEADER& IB=IBH[ibIdx];
        const uint64_t ibAbs=bufBase+IB.DataOffset;
        if(!inBounds(fsz,ibAbs,IB.SizeBytes))
        { fprintf(stderr,"  IB OOB\n"); continue; }

        // SubsetOffset é RELATIVO a H->HeaderSize (início do NonBufferData)
        // Confirmado pelo DXUT: pStaticMeshData + SubsetOffset
        // onde pStaticMeshData = pData + HeaderSize
        const uint64_t siAbs=H->HeaderSize+M.SubsetOffset;
        if(!inBounds(fsz,siAbs,(uint64_t)M.NumSubsets*sizeof(int32_t)))
        { fprintf(stderr,"  SubsetOffset OOB\n"); continue; }

        Attrib layout=decodeDecl(VB.Decl,(int)VB.StrideBytes);
        if(layout.posOfs<0) layout.posOfs=0;

        const uint8_t* vbd=B+vbAbs;
        const uint8_t* ibd=B+ibAbs;
        const bool     i32=(IB.IndexType==IT_32BIT);
        const int      stride=(int)VB.StrideBytes;

        auto rIdx=[&](uint64_t p)->uint32_t{
            if(i32){uint32_t v;memcpy(&v,ibd+p*4,4);return v;}
            else   {uint16_t v;memcpy(&v,ibd+p*2,2);return(uint32_t)v;}
        };

        Mesh mesh;
        mesh.name=mname; mesh.vbIdx=vbIdx;
        mesh.verts.reserve((size_t)VB.NumVertices);
        for(uint64_t vi=0;vi<VB.NumVertices;vi++)
        {
            const uint8_t* vp=vbd+vi*stride;
            Vert vt={};
            rv3(vp,layout.posOfs,layout.posT,vt.x,vt.y,vt.z);
            if(layout.nrmOfs>=0) rv3(vp,layout.nrmOfs,layout.nrmT,vt.nx,vt.ny,vt.nz);
            if(layout.texOfs>=0) rv2(vp,layout.texOfs,layout.texT,vt.u,vt.v);
            mesh.verts.push_back(vt);
        }

        const int32_t* SI=(const int32_t*)(B+siAbs);
        for(uint32_t si=0;si<M.NumSubsets;si++)
        {
            int32_t gIdx=SI[si];
            if(gIdx<0||(uint32_t)gIdx>=H->NumTotalSubsets)
            { fprintf(stderr,"  bad subset idx %d\n",gIdx); continue; }

            const SDKMESH_SUBSET& SS=AS[(uint32_t)gIdx];
            if(SS.PrimitiveType!=PT_TRIANGLE_LIST && SS.PrimitiveType!=PT_TRIANGLE_STRIP)
            { printf("  subset '%s' primType=%u skip\n",safeStr(SS.Name,MAX_SUBSET_NAME).c_str(),SS.PrimitiveType); continue; }
            if(SS.IndexStart+SS.IndexCount>IB.NumIndices)  { fprintf(stderr,"  IB range\n"); continue; }
            if(SS.VertexStart+SS.VertexCount>VB.NumVertices){ fprintf(stderr,"  VB range\n"); continue; }

            SubMesh sm;
            sm.name=safeStr(SS.Name,MAX_SUBSET_NAME);
            if(SS.MaterialID<H->NumMaterials && !mats[SS.MaterialID].diff.empty())
            { sm.diffTex=fileOnly(mats[SS.MaterialID].diff); sm.matId=fileNoExt(mats[SS.MaterialID].diff); }

            if(SS.PrimitiveType==PT_TRIANGLE_LIST)
            {
                sm.idx.reserve((size_t)SS.IndexCount);
                for(uint64_t ii=0;ii+2<SS.IndexCount;ii+=3)
                    for(int k=0;k<3;k++)
                        sm.idx.push_back(rIdx(SS.IndexStart+ii+k)+(uint32_t)SS.VertexStart);
            }
            else
            {
                for(uint64_t ii=0;ii+2<SS.IndexCount;ii++)
                {
                    uint32_t i0=rIdx(SS.IndexStart+ii),i1=rIdx(SS.IndexStart+ii+1),i2=rIdx(SS.IndexStart+ii+2);
                    if(i0==i1||i1==i2||i0==i2) continue;
                    if(ii&1) std::swap(i0,i1);
                    sm.idx.push_back(i0+(uint32_t)SS.VertexStart);
                    sm.idx.push_back(i1+(uint32_t)SS.VertexStart);
                    sm.idx.push_back(i2+(uint32_t)SS.VertexStart);
                }
            }

            if(!sm.idx.empty()) mesh.subs.push_back(std::move(sm));
        }

        printf("  → %zu verts, %zu subs\n",mesh.verts.size(),mesh.subs.size());
        out.push_back(std::move(mesh));
    }
    return !out.empty();
}

// ============================================================
// Export OBJ + MTL
// VBs partilhados são escritos uma única vez
// ============================================================

bool exportOBJ(const std::vector<Mesh>& meshes, const char* path)
{
    FILE* f=fopen(path,"w");
    if(!f){ fprintf(stderr,"Cannot write: %s\n",path); return false; }

    std::string mp=path;
    { auto d=mp.rfind('.'); if(d!=std::string::npos) mp=mp.substr(0,d); mp+=".mtl"; }
    auto sl=mp.rfind('/');
    std::string mb=(sl!=std::string::npos)?mp.substr(sl+1):mp;

    FILE* fm=fopen(mp.c_str(),"w");

    fprintf(f,"# SDKMesh → OBJ\nmtllib %s\n\n",mb.c_str());

    // Passagem 1 — vértices, uma vez por VB único
    std::unordered_map<uint32_t,uint32_t> vbOfs;
    uint32_t nextOfs=1;
    for(const Mesh& m:meshes)
    {
        if(vbOfs.count(m.vbIdx)) continue;
        vbOfs[m.vbIdx]=nextOfs;
        for(const Vert& v:m.verts) fprintf(f,"v  %.6f %.6f %.6f\n",v.x,v.y,v.z);
        for(const Vert& v:m.verts) fprintf(f,"vn %.6f %.6f %.6f\n",v.nx,v.ny,v.nz);
        for(const Vert& v:m.verts) fprintf(f,"vt %.6f %.6f\n",v.u,1.f-v.v);
        nextOfs+=(uint32_t)m.verts.size();
    }
    fprintf(f,"\n");

    // Passagem 2 — faces
    std::unordered_map<std::string,bool> wmat;
    for(const Mesh& m:meshes)
    {
        uint32_t vo=vbOfs[m.vbIdx];
        fprintf(f,"o %s\n",m.name.c_str());
        for(const SubMesh& sm:m.subs)
        {
            if(!sm.matId.empty())
            {
                if(fm && !wmat.count(sm.matId))
                { fprintf(fm,"newmtl %s\n  map_Kd %s\n\n",sm.matId.c_str(),sm.diffTex.c_str()); wmat[sm.matId]=true; }
                fprintf(f,"usemtl %s\n",sm.matId.c_str());
            }
            fprintf(f,"g %s\n",sm.name.c_str());
            for(size_t i=0;i+2<sm.idx.size();i+=3)
            {
                uint32_t a=sm.idx[i]+vo, b=sm.idx[i+1]+vo, c=sm.idx[i+2]+vo;
                fprintf(f,"f %u/%u/%u %u/%u/%u %u/%u/%u\n",a,a,a,b,b,b,c,c,c);
            }
        }
        fprintf(f,"\n");
    }

    if(fm) fclose(fm);
    fclose(f);
    printf("Exported: %s\n",path);
    return true;
}

// ============================================================
// Main
// ============================================================

int main(int argc, char* argv[])
{
    if(argc<3)
    { printf("Usage: sdkmesh_convert <input.sdkmesh> <output.obj>\n"); return 1; }

    std::vector<Mesh> meshes;
    if(!loadSDKMesh(argv[1],meshes)) return 1;

    size_t tv=0,tt=0;
    for(const Mesh& m:meshes)
    { tv+=m.verts.size(); for(const SubMesh& s:m.subs) tt+=s.idx.size()/3; }
    printf("Total: %zu verts, %zu tris, %zu meshes\n",tv,tt,meshes.size());

    return exportOBJ(meshes,argv[2])?0:1;
}
