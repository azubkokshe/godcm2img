#pragma once

#include <iostream>
#include "dcmtk/dcmdata/dctk.h"

using namespace std;

class DCM2PNM {
public:
  DCM2PNM();
  ~DCM2PNM();
  void* convert(int *outSize, char *fileData, int fileSize, unsigned int frameNumber, int outputFormat);
  char* getError();
private:
  string errorString;
  void error(string str);
  void warning(string str);
};
