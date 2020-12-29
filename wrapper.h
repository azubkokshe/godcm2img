#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void* New();
void Destroy(void* p);
void* Convert(void* p, int *outSize, char *fileData, int fileSize, unsigned int frameNumber, int outputFormat, unsigned int quality,
                int useClip, int left, int top, unsigned int width, unsigned int height,
                int scaleType, unsigned int scaleWidth, unsigned int scaleHeight);

char* GetError(void* p);
#ifdef __cplusplus
} // extern "C"
#endif