#pragma once

#include <stdio.h>

// set targetSize to 0 if you want to get the file size
int readBinaryFile2(const char* filename, void* pData, size_t& targetSize) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Failed to open file '%s'\n", filename);
        return -1;
    }
    if (targetSize == 0) {
        fseek(fp, 0, SEEK_END);     /* move fp to end of file */
        targetSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);     /* move fp back to beginning of file */
    }
    size_t readSize = fread(pData, sizeof(unsigned char), targetSize, fp);
    if (readSize != targetSize) {
        printf("read size (%zu) != target size (%zu). The file '%s' might be incomplete!\n", readSize, targetSize, filename);
    }
    fclose(fp);
    return 0;
}