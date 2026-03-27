
#include "binaryStream.hpp"
#include <cstring>
// ============================================================
//  BinaryStream — FILE*, little-endian explícito
// ============================================================

// Portable little-endian helpers
static uint32_t to_le32(uint32_t v)
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return __builtin_bswap32(v);
#else
    return v;
#endif
}

static uint32_t from_le32(uint32_t v) { return to_le32(v); }

BinaryStream::BinaryStream(const std::string &path, const char *mode)
{
    fp_ = fopen(path.c_str(), mode);
    if (!fp_)
        fprintf(stderr, "[BinaryStream] fopen failed: %s\n", path.c_str());
}

BinaryStream::~BinaryStream()
{
    if (fp_)
        fclose(fp_);
}

// ── Write ────────────────────────────────────────────────────
void BinaryStream::writeU8(uint8_t v)
{
    fwrite(&v, 1, 1, fp_);
}

void BinaryStream::writeU32(uint32_t v)
{
    uint32_t le = to_le32(v);
    fwrite(&le, 4, 1, fp_);
}

void BinaryStream::writeI32(int32_t v)
{
    writeU32((uint32_t)v);
}

void BinaryStream::writeF32(float v)
{
    uint32_t bits;
    memcpy(&bits, &v, 4);
    writeU32(bits);
}

void BinaryStream::writeStr(const std::string &s)
{
    fwrite(s.c_str(), 1, s.size() + 1, fp_);
}

void BinaryStream::writeRaw(const void *data, size_t bytes)
{
    fwrite(data, 1, bytes, fp_);
}

// ── Read ─────────────────────────────────────────────────────
uint8_t BinaryStream::readU8()
{
    uint8_t v = 0;
    fread(&v, 1, 1, fp_);
    return v;
}

uint32_t BinaryStream::readU32()
{
    uint32_t v = 0;
    fread(&v, 4, 1, fp_);
    return from_le32(v);
}

int32_t BinaryStream::readI32()
{
    return (int32_t)readU32();
}

float BinaryStream::readF32()
{
    uint32_t bits = readU32();
    float v;
    memcpy(&v, &bits, 4);
    return v;
}

std::string BinaryStream::readStr()
{
    std::string r;
    r.reserve(64);
    while (true)
    {
        uint8_t c = readU8();
        if (c == 0)
            break;
        r += (char)c;
    }
    return r;
}

void BinaryStream::readRaw(void *data, size_t bytes)
{
    fread(data, 1, bytes, fp_);
}

// ── Seek / Tell ──────────────────────────────────────────────
long BinaryStream::tell() const
{
    return ftell(fp_);
}

void BinaryStream::seek(long pos)
{
    fseek(fp_, pos, SEEK_SET);
}

long BinaryStream::size()
{
    long cur = ftell(fp_);
    fseek(fp_, 0, SEEK_END);
    long s = ftell(fp_);
    fseek(fp_, cur, SEEK_SET);
    return s;
}

bool BinaryStream::eof() const
{
    return feof(fp_) != 0;
}