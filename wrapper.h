#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void* New();
void Destroy(void* p);
void* Convert(void* p, int *outSize, char *fileData, int fileSize, unsigned int frameNumber, int outputFormat);

char* GetError(void* p);
#ifdef __cplusplus
} // extern "C"
#endif