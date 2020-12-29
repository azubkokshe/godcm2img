#pragma once

#include <iostream>

using namespace std;

class DCM2PNM {
public:
  DCM2PNM();
  ~DCM2PNM();
  void* convert(int *outSize, char *fileData, int fileSize, unsigned int opt_frame, int outputFormat,
                                        unsigned int opt_quality, int opt_useClip, int opt_left, int opt_top, unsigned int opt_width,
                                        unsigned int opt_height, int opt_scaleType, unsigned int opt_scale_width, unsigned int opt_scale_height);
  char* getError();
private:
  string errorString;
  void error(string str);
  void warning(string str);
};
