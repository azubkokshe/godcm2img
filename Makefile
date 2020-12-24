libdcm2pnm.so: godcm2img/dcm2pnm.cpp godcm2img/wrapper.cpp
	clang++ -o libdcm2pnm.so godcm2img/dcm2pnm.cpp godcm2img/wrapper.cpp -pthread -ldcmdata -lz -loflog -lofstd -ldcmjpeg -ldcmimgle -ldcmimage \
	-std=c++17 -O3 -Wall -Wextra -fPIC -shared