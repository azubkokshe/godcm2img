libdcm2pnm.so: dcm2pnm.cpp wrapper.cpp
	g++ -o libdcm2pnm.so dcm2pnm.cpp wrapper.cpp -pthread -ldcmdata -lz -loflog -lofstd -ldcmjpeg -ldcmimgle -ldcmimage \
-std=c++17 -O3 -Wall -Wextra -fPIC -shared

#	clang++ -o libdcm2pnm.so godcm2img/dcm2pnm.cpp godcm2img/wrapper.cpp -pthread -ldcmdata -lz -loflog -lofstd -ldcmjpeg -ldcmimgle -ldcmimage \
#	-std=c++17 -O3 -Wall -Wextra -fPIC -shared