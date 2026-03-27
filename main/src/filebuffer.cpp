#include "filebuffer.hpp"
#include "utils.hpp"
#include "interpreter.hpp"
#include "raylib.h"
#include <cstdlib>
 
 

bool FileBuffer::load(const char *path)

{
    data.clear();

   int fileSize = 0;
    unsigned char *fileData = LoadFileData(path, &fileSize);
    if (!fileData || fileSize <= 0)
    {
        if (fileData)
           free(fileData);
        return false;
    }

    data.assign(fileData, fileData + fileSize);
    data.push_back(0); // Keep NUL terminator for C-style loaders.
    free(fileData);
    return true;
}