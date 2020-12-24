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
  AsDCM2PNM(p)->~DCM2PNM();
}

char* GetError(void* p) {
  return AsDCM2PNM(p)->getError();
}

void* Convert(void* p, int *outSize, const char *opt_ifname, unsigned int frameNumber, int outputFormat) {
  return AsDCM2PNM(p)->convert(outSize, opt_ifname, frameNumber, outputFormat);
}

