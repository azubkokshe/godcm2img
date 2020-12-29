#include <iostream>

#include "wrapper.h"
#include "dcm2pnm.hpp"

void* New() {
  auto p = new DCM2PNM();
  return p;
}

DCM2PNM* AsDCM2PNM(void* p) {
    return reinterpret_cast<DCM2PNM*>(p);
}

void Destroy(void* p) {
    if (p != NULL)
        free(AsDCM2PNM(p));
}

char* GetError(void* p) {
  return AsDCM2PNM(p)->getError();
}

void* Convert(void* p, int *outSize, char *fileData, int fileSize, unsigned int frameNumber, int outputFormat, unsigned int quality,
                int useClip, int left, int top, unsigned int width, unsigned int height,
                int scaleType, unsigned int scaleWidth, unsigned int scaleHeight) {
  return AsDCM2PNM(p)->convert(outSize, fileData, fileSize, frameNumber, outputFormat, quality, useClip,
                                left, top, width, height, scaleType, scaleWidth, scaleHeight);
}

